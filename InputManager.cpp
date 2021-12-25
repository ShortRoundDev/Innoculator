#include "InputManager.h"
#include "SystemManager.h"
#include "GraphicsManager.h"
#include "Camera.h"

std::unique_ptr<InputManager> InputManager::instance;
ULONGLONG InputManager::startOffset;

void InputManager::init()
{
	instance = std::make_unique<InputManager>();
	startOffset = GetTickCount64();
}

InputManager::InputManager()
{
	ZeroMemory(keys, sizeof(keys));

	status = true;

	mouse = std::make_unique<Mouse>();
	mouse->SetWindow(SYSTEM->hwnd);
	mouse->SetMode(Mouse::MODE_RELATIVE);
}

InputManager::~InputManager()
{

}

void InputManager::onKeyDown(UINT key)
{
	if (!keys[key].down)
	{
		keys[key].edge = true;
		keys[key].lastDown = startOffset - GetTickCount64();
	}
	keys[key].down = true;
}

void InputManager::onKeyUp(UINT key)
{
	if (keys[key].down)
	{
		keys[key].edge = true;
		keys[key].lastDown = 0;
	}
	keys[key].down = false;
}


void InputManager::onMouseMove(int x, int y)
{
	
	float dX = (float)x;
	float dY = (float)y;

	Camera* camera = GRAPHICS->camera;

	float sensitivity = 0.1f;

	dX *= sensitivity;
	dY *= sensitivity;

	camera->rotation.y += dX;
	camera->rotation.x += dY;

	if (camera->rotation.x > 89.0f)
	{
		camera->rotation.x = 89.0f;
	}
	if (camera->rotation.x < -89.0f)
	{
		camera->rotation.x = -89.0f;
	}
}

void InputManager::update()
{
	for (int i = 0; i < 255; i++) {
		keys[i].edge = false;
	}
	Mouse::State state = mouse->GetState();

	if (state.x == 0 && state.y == 0)
		return;

	onMouseMove(state.x, state.y);
}

bool InputManager::isDown(UINT key)
{
	return keys[key].down;
}