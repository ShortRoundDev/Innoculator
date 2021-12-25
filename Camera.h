#pragma once

#include <DirectXMath.h>
using namespace DirectX;

constexpr float TO_RAD = 0.0174532925f;

class Camera
{
public:
	Camera();
	~Camera();

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);

	XMFLOAT3 position;
	XMFLOAT3 rotation;

	XMMATRIX viewMatrix;
	XMMATRIX orthoViewMatrix;
	XMMATRIX skyViewMatrix;

	void update();
};
