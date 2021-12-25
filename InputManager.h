#pragma once

#include "base.h"

#include <memory>
#include <Mouse.h>

#define INPUT (InputManager::instance)

#define KEYS (INPUT->keys)
#define ISDOWN(x) (INPUT->isDown((x)))

using namespace DirectX;

struct KeyState {
	bool down;
	bool edge;
	ULONGLONG lastDown;
};

class InputManager
{
public:
	static std::unique_ptr<InputManager> instance;
	static void init();

	static ULONGLONG startOffset;
	
	bool status;

	InputManager();
	~InputManager();

	void onKeyDown(UINT key);
	void onKeyUp(UINT key);
	void onMouseMove(int x, int y);

	KeyState keys[256];
	std::unique_ptr<Mouse> mouse;

	void update();

	bool isDown(UINT key);
};

