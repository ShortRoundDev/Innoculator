#include "Program.h"

#include "SystemManager.h"
#include "GameManager.h"
#include "InputManager.h"
#include "GraphicsManager.h"

#include <iostream>

ULONGLONG ticks = 0;

int main(int argc, char** argv)
{
	if (!init()) {
		return 0;
	}
	run();
	return 1;
}

bool init()
{
	SystemManager::init();
	InputManager::init();
	GraphicsManager::init();
	GameManager::init();

	return true;
}

void update()
{
	INPUT->update();
	SYSTEM->update();
	GAME->update();
	GRAPHICS->update();
}

void draw()
{
	GRAPHICS->clear();
	GRAPHICS->updateGlobalCBuffer(false);

	GAME->draw();

	GRAPHICS->swap();
}

void run()
{
	while (SYSTEM->running) {
		//bad, replace this with accumulator later
		ULONGLONG ticksNow = GetTickCount64();
		if (ticksNow - ticks < 15)
		{
			continue;
		}
		ticks = ticksNow;

		update();
		draw();
	}
}