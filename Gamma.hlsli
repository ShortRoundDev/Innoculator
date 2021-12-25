#ifndef __GAMMA_HLSLI__
#define __GAMMA_HLSLI__

float3 Gamma3(float3 color)
{
	float gamma = 2.2f;
	return pow(color, float3(gamma, gamma, gamma));
}

float4 Gamma4(float4 color)
{
	float gamma = 2.2f;
	return float4(pow(color, float3(gamma, gamma, gamma)), color.w);
}

#endif