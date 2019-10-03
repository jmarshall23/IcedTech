// RaytracingUtility.h
//

float3 RayPlaneIntersection(float3 planeOrigin, float3 planeNormal, float3 rayOrigin, float3 rayDirection)
{
	float t = dot(-planeNormal, rayOrigin - planeOrigin) / dot(planeNormal, rayDirection);
	return rayOrigin + rayDirection * t;
}

bool Inverse2x2(float2x2 mat, out float2x2 inverse)
{
	float determinant = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];

	float rcpDeterminant = rcp(determinant);
	inverse[0][0] = mat[1][1];
	inverse[1][1] = mat[0][0];
	inverse[1][0] = -mat[0][1];
	inverse[0][1] = -mat[1][0];
	inverse = rcpDeterminant * inverse;

	return abs(determinant) > 0.00000001;
}

/* TODO: Could be precalculated per triangle
Using implementation described in PBRT, finding the partial derivative of the (change in position)/(change in UV coordinates)
a.k.a dp/du and dp/dv

Given the 3 UV and 3 triangle points, this can be represented as a linear equation:

(uv0.u - uv2.u, uv0.v - uv2.v)   (dp/du)   =     (p0 - p2)
(uv1.u - uv2.u, uv1.v - uv2.v)   (dp/dv)   =     (p1 - p2)

To solve for dp/du, we invert the 2x2 matrix on the left side to get

(dp/du)   = (uv0.u - uv2.u, uv0.v - uv2.v)^-1  (p0 - p2)
(dp/dv)   = (uv1.u - uv2.u, uv1.v - uv2.v)     (p1 - p2)
*/
void CalculateTrianglePartialDerivatives(float2 uv0, float2 uv1, float2 uv2, float3 p0, float3 p1, float3 p2, out float3 dpdu, out float3 dpdv)
{
	float2x2 linearEquation;
	linearEquation[0] = uv0 - uv2;
	linearEquation[1] = uv1 - uv2;

	float2x3 pointVector;
	pointVector[0] = p0 - p2;
	pointVector[1] = p1 - p2;
	float2x2 inverse;
	Inverse2x2(linearEquation, inverse);
	dpdu = pointVector[0] * inverse[0][0] + pointVector[1] * inverse[0][1];
	dpdv = pointVector[0] * inverse[1][0] + pointVector[1] * inverse[1][1];
}

/*
Using implementation described in PBRT, finding the derivative for the UVs (dU, dV)  in both the x and y directions

Given the original point and the offset points (pX and pY) + the partial derivatives, the linear equation can be formed:
Note described only with pX, but the same is also applied to pY

( dpdu.x, dpdv.x)          =   (pX.x - p.x)
( dpdu.y, dpdv.y)   (dU)   =   (pX.y - p.y)
( dpdu.z, dpdv.z)   (dV)   =   (pX.z - p.z)

Because the problem is over-constrained (3 equations and only 2 unknowns), we pick 2 channels, and solve for dU, dV by inverting the matrix

dU    =   ( dpdu.x, dpdv.x)^-1  (pX.x - p.x)
dV    =   ( dpdu.y, dpdv.y)     (pX.y - p.y)
*/

void CalculateUVDerivatives(float3 normal, float3 dpdu, float3 dpdv, float3 p, float3 pX, float3 pY, out float2 ddX, out float2 ddY)
{
	int2 indices;
	float3 absNormal = abs(normal);
	if (absNormal.x > absNormal.y && absNormal.x > absNormal.z)
	{
		indices = int2(1, 2);
	}
	else if (absNormal.y > absNormal.z)
	{
		indices = int2(0, 2);
	}
	else
	{
		indices = int2(0, 1);
	}

	float2x2 linearEquation;
	linearEquation[0] = float2(dpdu[indices.x], dpdv[indices.x]);
	linearEquation[1] = float2(dpdu[indices.y], dpdv[indices.y]);

	float2x2 inverse;
	Inverse2x2(linearEquation, inverse);
	float2 pointOffset = float2(pX[indices.x] - p[indices.x], pX[indices.y] - p[indices.y]);
	ddX = abs(mul(inverse, pointOffset));

	pointOffset = float2(pY[indices.x] - p[indices.x], pY[indices.y] - p[indices.y]);
	ddY = abs(mul(inverse, pointOffset));
}