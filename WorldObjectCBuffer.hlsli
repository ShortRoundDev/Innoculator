#ifndef __WORLD_OBJECT_CBUFFER_HLSLI__
#define __WORLD_OBJECT_CBUFFER_HLSLI__

cbuffer WorldObjectCBuffer : register(b1)
{
	float4 entityPos;
};

#endif