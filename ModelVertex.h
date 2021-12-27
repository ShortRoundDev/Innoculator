#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#pragma pack(0)
struct ModelVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};