#include "PixelHeader.hlsli"
#include "Gamma.hlsli"

Texture2D shaderTexture;
SamplerState sampleType;

float4 Pixel(PixelInput input) : SV_TARGET
{
	float4 texColor = shaderTexture.Sample(sampleType, input.tex);
	texColor = Gamma4(texColor);

	return  texColor;
}