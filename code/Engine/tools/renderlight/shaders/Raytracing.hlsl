//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#define HLSL
#include "RaytracingHlslCompat.h"
#include "RaytracingUtility.h"

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);
ByteAddressBuffer Indices : register(t1, space0);
StructuredBuffer<Vertex> Vertices : register(t2, space0);

SamplerState      albedoSample;
Texture2D<float4> albedoTexture : register(t3);

ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);
ConstantBuffer<CubeConstantBuffer> g_cubeCB : register(b1);

// Utility function to get a vector perpendicular to an input vector 
//    (from "Efficient Construction of Perpendicular Vectors Without Branching")
float3 getPerpendicularVector(float3 u)
{
	float3 a = abs(u);
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}

// Takes our seed, updates it, and returns a pseudorandom float in [0..1]
float nextRand(inout uint s)
{
	s = (1664525u * s + 1013904223u);
	return float(s & 0x00FFFFFF) / float(0x01000000);
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 getCosHemisphereSample(uint randSeed, float3 hitNorm)
{
	// Get 2 random numbers to select our sample with
	float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));

	// Cosine weighted hemisphere sample from RNG
	float3 bitangent = getPerpendicularVector(hitNorm);
	float3 tangent = cross(bitangent, hitNorm);
	float r = sqrt(randVal.x);
	float phi = 2.0f * 3.14159265f * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + hitNorm.xyz * sqrt(max(0.0, 1.0f - randVal.x));
}

uint3 Load3x32BitIndices(uint offsetBytes)
{
	// Load first 2 indices
	return Indices.Load3(offsetBytes);
}

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
	float4 normal;
	float4 hitlocation;
	float2 lightmapuv;
	int unused;
	int sliceIdx;
};

// Retrieve hit world position.
float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}

// Retrieve attribute at a hit position interpolated from vertex attributes using the hit's barycentrics.
float3 HitAttribute(float3 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
inline void GenerateCameraRay(int sliceIdx, uint2 index, out float3 origin, out float3 direction)
{
    float2 xy = index + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld[sliceIdx]);

    world.xyz /= world.w;
    origin = g_sceneCB.cameraPosition.xyz;
    direction = normalize(world.xyz - origin);
}

// Diffuse lighting calculation.
//float4 CalculateDiffuseLighting(float3 hitPosition, float3 normal)
//{
//    float3 pixelToLight = normalize(g_sceneCB.lightPosition.xyz - hitPosition);
//
//    // Diffuse contribution.
//    float fNDotL = max(0.0f, dot(pixelToLight, normal));
//
//    return g_cubeCB.albedo * g_sceneCB.lightDiffuseColor * fNDotL;
//}

[shader("raygeneration")]
void MyRaygenShader()
{
	RayPayload payload[6];
	
	for (int i = 0; i < 6; i++)
	{
		payload[i].color = float4(0, 0, 0, 0);
		payload[i].normal = float4(0, 0, 0, 0);
		payload[i].lightmapuv = float2(0, 0);
		payload[i].unused = 0;
		payload[i].sliceIdx = i;
	}

	for (int i = 0; i < 6; i++)
	{
		float3 rayDir;
		float3 origin;

		// Generate a ray for a camera pixel corresponding to an index from the dispatched 2D grid.
		GenerateCameraRay(i, DispatchRaysIndex(), origin, rayDir);

		// Trace the ray.
		// Set the ray's extents.
		RayDesc ray;
		ray.Origin = origin;
		ray.Direction = rayDir;
		// Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
		// TMin should be kept small to prevent missing geometry at close contact areas.
		ray.TMin = 0.001;
		ray.TMax = 100000.0;
		
		TraceRay(Scene, 0, ~0, 0, 1, 0, ray, payload[i]);

		//{
		//	ray.Origin = payload[i].hitlocation;
		//	ray.Direction = payload[i].normal;
		//	// Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
		//	// TMin should be kept small to prevent missing geometry at close contact areas.
		//	ray.TMin = 0.001;
		//	ray.TMax = 5.00;
		//
		//	// We're going to tell our ray to never run the closest-hit shader and to
		//	//     stop as soon as we find *any* intersection
		//	uint rayFlags = RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
		//
		//	// Trace our ray. 
		//	TraceRay(Scene, rayFlags, 0xFF, 0, 1, 0, ray, payload[i]);
		//
		//	if(payload[i].color.w != 1.0)
		//		payload[i].color.xyz *= 0.1;
		//}
	}

	for (int i = 0; i < 6; i++)
	{
		RenderTarget[payload[i].lightmapuv].rgb += payload[i].color;
	}
	

    // Write the raytraced color to the output texture.
    //RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

float attenuation(float r, float f, float d) {
	d = d * (1.0 + g_sceneCB.lightInfo.y);
	return clamp((r / (d * d)) * 32, 0, 1);
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{
    float3 hitPosition = HitWorldPosition();
	float3 worldPosition = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

	// Find first index location
	const uint index_size = 4;
	const uint indices_per_triangle = 3;
	const uint triangle_idx_stride = indices_per_triangle * index_size;

	uint base_idx = PrimitiveIndex() * triangle_idx_stride;

    // Load up 3 16 bit indices for the triangle.
    const uint3 indices = Load3x32BitIndices(base_idx);
	
	float3 vertexPosition[3] = {
		Vertices[indices[0]].position,
		Vertices[indices[1]].position,
		Vertices[indices[2]].position
	};
	
    // Retrieve corresponding vertex normals for the triangle vertices.
    float3 vertexNormals[3] = { 
        Vertices[indices[0]].normal, 
        Vertices[indices[1]].normal, 
        Vertices[indices[2]].normal 
    };
	
	float3 vertexUV[3] = {
		Vertices[indices[0]].texcoord,
		Vertices[indices[1]].texcoord,
		Vertices[indices[2]].texcoord
	};

	float3 e1 = vertexPosition[1] - vertexPosition[0];
	float3 e2 = vertexPosition[2] - vertexPosition[0];
	float3 normal = normalize(cross(e2, e1));

	if (vertexUV[0].z != 1.0f && vertexUV[1].z != 1.0f && vertexUV[2].z != 1.0f)
		return;
	
	uint2 threadID = DispatchRaysIndex().xy;
	float3 ddxOrigin, ddxDir, ddyOrigin, ddyDir;
	//GenerateCameraRay(uint2(threadID.x + 1, threadID.y), ddxOrigin, ddxDir);
	//GenerateCameraRay(uint2(threadID.x, threadID.y + 1), ddyOrigin, ddyDir);
	
    // Compute the triangle's normal.
    // This is redundant and done for illustration purposes 
    // as all the per-vertex normals are the same and match triangle's normal in this sample. 
    float3 triangleNormal = HitAttribute(vertexNormals, attr);
	float3 xOffsetPoint = RayPlaneIntersection(worldPosition, triangleNormal, ddxOrigin, ddxDir);
	float3 yOffsetPoint = RayPlaneIntersection(worldPosition, triangleNormal, ddyOrigin, ddyDir);
	
	
	float3 bary = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
	float2 uv = bary.x * vertexUV[0] + bary.y * vertexUV[1] + bary.z * vertexUV[2];
	
	float3 dpdu, dpdv;
	float2 ddx, ddy;
	
	{
		CalculateTrianglePartialDerivatives(vertexUV[0], vertexUV[1], vertexUV[2], vertexPosition[0], vertexPosition[1], vertexPosition[2], dpdu, dpdv);
		CalculateUVDerivatives(triangleNormal, dpdu, dpdv, worldPosition, xOffsetPoint, yOffsetPoint, ddx, ddy);
	}

	float3 lightradius = g_sceneCB.lightInfo.x;

	float3 lightVector = worldPosition - g_sceneCB.cameraPosition;
	float lightDistance = length(lightVector);

	float falloff = attenuation(lightradius, 1.0, lightDistance);
	float3 L = normalize(lightVector);

	uv.y = 1.0 - uv.y;

	payload.color.w = 0.0f;
	payload.lightmapuv.xy = uv * g_sceneCB.lightInfo.z;
	payload.normal.xyz = normal;
	payload.hitlocation.xyz = hitPosition;
	payload.color.xyz = g_sceneCB.lightColor.xyz; // max(dot(normal, L), 0.0f); // (falloff /* * g_sceneCB.lightColor.xyz */);

	//RenderTarget[uv * 4096].rgb = clamp(RenderTarget[uv * 4096].rgb, 0, 1) + payload.color.rgb;
}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    payload.color.w = 1.0;
}

#endif // RAYTRACING_HLSL