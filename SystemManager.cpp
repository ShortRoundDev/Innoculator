#include "SystemManager.h"
#include "InputManager.h"

#include <combaseapi.h>
#include <Mouse.h>

std::unique_ptr<SystemManager> SystemManager::instance;

using namespace DirectX;

void SystemManager::init()
{
	instance = std::make_unique<SystemManager>();
}

SystemManager::SystemManager() :
	fullScreen(false),
	screenWidth(1024),
	screenHeight(768)
{
	status = initWindow();

	quill::enable_console_colours();
	quill::start();
	logger = quill::get_logger();
	logger->set_log_level(quill::LogLevel::TraceL3);
	logger->init_backtrace(2, quill::LogLevel::Critical);
}

SystemManager::~SystemManager()
{

}

bool SystemManager::initWindow()
{

	const WCHAR* title = L"Innoculator";

	hInstance = GetModuleHandle(NULL);
	WNDCLASSEX wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	if (fullScreen)
	{
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
	}

	int posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2,
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	/*}*/

	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		title,
		title,
		WS_TILEDWINDOW,
		posX, posY,
		screenWidth, screenHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemManager::messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
		INPUT->onKeyDown((UINT)wparam);
		return 0;
	case WM_KEYUP:
		INPUT->onKeyUp((UINT)wparam);
		return 0;
	case WM_ACTIVATEAPP:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(umsg, wparam, lparam);
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

bool SystemManager::update()
{
	MSG msg = { };
	ZeroMemory(&msg, sizeof(msg));

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			return true;
		}
	}
	return false;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		SYSTEM->running = false;
		return 0;
	default:
		return SYSTEM->messageHandler(hWnd, uMsg, wParam, lParam);
	}
}