#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct GlobalCBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX invWorld;
	XMFLOAT4 cameraPos;
};