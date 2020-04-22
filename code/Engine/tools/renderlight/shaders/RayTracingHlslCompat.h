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

#ifndef RAYTRACINGHLSLCOMPAT_H
#define RAYTRACINGHLSLCOMPAT_H

#ifdef HLSL
#include "HlslCompat.h"
#else
using namespace DirectX;

// Shader will use byte encoding to access indices.
typedef UINT32 Index;
#endif

#define MAX_POLYMERTRX_LIGHTS			8

struct SceneLight
{
	XMVECTOR lightPosition;
	XMVECTOR lightDiffuseColor;
};

struct SceneConstantBuffer
{
    XMMATRIX projectionToWorld[6];
    XMVECTOR cameraPosition;
	XMVECTOR lightColor;
	XMVECTOR lightInfo;
	int lightId;
};

struct CubeConstantBuffer
{
    XMFLOAT4 albedo;
};

struct Vertex
{
    XMFLOAT3 position;
	XMFLOAT3 texcoord;
    XMFLOAT3 normal;
#ifndef HLSL
	Vertex operator+(Vertex s)
	{
		position.x = position.x + s.position.x;
		position.y = position.y + s.position.y;
		position.z = position.z + s.position.z;

		texcoord.x = texcoord.x + s.texcoord.x;
		texcoord.y = texcoord.y + s.texcoord.y;
		texcoord.z = texcoord.z + s.texcoord.z;

		normal.x = normal.x + s.normal.x;
		normal.y = normal.y + s.normal.y;
		normal.z = normal.z + s.normal.z;
		return *this;
	}

	Vertex operator*(Vertex s)
	{
		position.x = position.x * s.position.x;
		position.y = position.y * s.position.y;
		position.z = position.z * s.position.z;

		texcoord.x = texcoord.x * s.texcoord.x;
		texcoord.y = texcoord.y * s.texcoord.y;
		texcoord.z = texcoord.z * s.texcoord.z;

		normal.x = normal.x * s.normal.x;
		normal.y = normal.y * s.normal.y;
		normal.z = normal.z * s.normal.z;
		return *this;
	}

	Vertex operator*(double s)
	{
		position.x = position.x * s;
		position.y = position.y * s;
		position.z = position.z * s;

		texcoord.x = texcoord.x * s;
		texcoord.y = texcoord.y * s;
		texcoord.z = texcoord.z * s;

		normal.x = normal.x * s;
		normal.y = normal.y * s;
		normal.z = normal.z * s;
		return *this;
	}
#endif
};

#endif // RAYTRACINGHLSLCOMPAT_H
