#include "Camera.h"

#include "InputManager.h"
#include "SystemManager.h"
#include "GameManager.h"

const float VEL = 0.02f;

Camera::Camera()
{
	position = XMFLOAT3(0.5f, 2.0f, -5.0f);
	rotation = XMFLOAT3(50, 0.0f, 0);
}

Camera::~Camera()
{

}

void Camera::setPosition(float x, float y, float z)
{
	position = { x, y, z };
}

void Camera::setRotation(float x, float y, float z)
{
	rotation = { x, y, z };
}

void Camera::update()
{
	XMVECTOR positionVec = XMLoadFloat3(&position);

	XMFLOAT3 look = { 0, 0, 1.0f };
	XMVECTOR lookVec = XMLoadFloat3(&look);

	XMFLOAT3 up = { 0, 1.0f, 0 };
	XMVECTOR upVec = XMLoadFloat3(&up);

	float	p = rotation.x * TO_RAD,
			y = rotation.y * TO_RAD,
			r = rotation.z * TO_RAD;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(p, y, r);

	lookVec = XMVector3TransformCoord(lookVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);
	lookVec = XMVectorAdd(positionVec, lookVec);

	XMFLOAT3 lookLog;
	XMStoreFloat3(&lookLog, lookVec);

	viewMatrix = XMMatrixLookAtLH(positionVec, lookVec, upVec);
	orthoViewMatrix = XMMatrixLookAtLH({ 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 });
	skyViewMatrix = XMMatrixLookAtLH(positionVec, lookVec, upVec);
}