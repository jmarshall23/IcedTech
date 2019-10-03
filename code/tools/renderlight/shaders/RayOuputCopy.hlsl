// RayOutputCopy.hlsl
//

RWTexture2D<float4> RenderTarget : register(u0);
RWTexture2D<float4> FinalRenderTarget : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 dtid : SV_DispatchThreadID )
{
	float4 colorValue = FinalRenderTarget[dtid.xy] + RenderTarget[dtid.xy];
	
	if (colorValue.x > 1.0 || colorValue.y > 1.0 || colorValue.z > 1.0)
	{
		float max;

		max = colorValue.r > colorValue.g ? colorValue.r : colorValue.g;
		max = max > colorValue.b ? max : colorValue.b;
		colorValue.r = colorValue.r * (1.0 / max);
		colorValue.g = colorValue.g * (1.0 / max);
		colorValue.b = colorValue.b * (1.0 / max);
	}
	FinalRenderTarget[dtid.xy] = colorValue;
	RenderTarget[dtid.xy] = float4(0, 0, 0, 0);
}