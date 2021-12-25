#pragma once

#include <memory>
#include "base.h"

#pragma warning(push, 0)
#include "quill/Quill.h"
#pragma warning(pop)

#define SYSTEM (SystemManager::instance)
#define LOGGER (SYSTEM->logger)

class SystemManager
{
public:
	static std::unique_ptr<SystemManager> instance;

	static void init();

	SystemManager();
	~SystemManager();

	bool initWindow();

	LRESULT CALLBACK messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	bool update();

	bool status;

	// ===== SYS VARS ===== //
	bool vsync;
	bool fullScreen;
	UINT16 screenWidth;
	UINT16 screenHeight;
	// ==================== //

	bool running = true;

	HWND hwnd;
	HINSTANCE hInstance;

	quill::Logger* logger;
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsh, WPARAM wParam, LPARAM lParam);
