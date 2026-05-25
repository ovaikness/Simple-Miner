#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/NamedProperties.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Window* Window::s_theWindow = nullptr;

LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE(window != nullptr, "Window was null!");

	InputSystem* input = window->GetConfig().m_inputSystem;
	
	GUARANTEE_OR_DIE(input != nullptr, "Windows InputSystem pointer was null!");

	switch (wmMessageCode)
	{
		case WM_KILLFOCUS:
		{
			input->HandleFocusLost();
			break;
		}
		case WM_SETFOCUS:
		{
			input->HandleFocusGained();
			break;
		}
		case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			UINT numFiles = DragQueryFileA(hDrop, 0xFFFFFFFF, nullptr, 0);

			for (UINT fileIndex = 0; fileIndex < numFiles; ++fileIndex)
			{
				UINT filePathLength = DragQueryFileA(hDrop, fileIndex, nullptr, 0);
				std::string filePath;
				filePath.resize(filePathLength);
				DragQueryFileA(hDrop, fileIndex, (LPSTR)&filePath[0], filePathLength + 1);
				EventArgs args;
				args.SetValue("FilePath", filePath);
				g_theEventSystem->FireEvent("FileDropped", args);
			}
			DragFinish(hDrop);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			EventArgs args;
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			args.SetValue("zDelta", std::to_string(zDelta));
			bool wasConsumed = g_theEventSystem->FireEvent("MouseScrolled", args);
			if (!wasConsumed)
			{
				wasConsumed = input->HandleScrollDelta(zDelta);
			}
			if (wasConsumed)
			{
				return 0;
			}
			return 0;
		}
		case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT)
			{
				SetCursor((HCURSOR)window->m_cursorHandle);
				return TRUE;
			}
			return 0;
		}

		case WM_CLOSE:
		{
			g_theEventSystem->FireEvent("Quit");
			return 0;
		}

		case WM_CHAR:
		{
			unsigned char keyCode = static_cast<unsigned char>(wParam);
			EventArgs args;
			std::string key = std::to_string(keyCode);
			args.SetValue("Key", keyCode);
			g_theEventSystem->FireEvent("KeyTyped", args);
			return 0;
		}

		case WM_KEYDOWN:
		{
			unsigned char keyCode = static_cast<unsigned char>(wParam);
			bool wasConsumed = false;

			EventArgs args;
			std::string key = std::to_string(keyCode);
			args.SetValue("Key", keyCode);
			wasConsumed = g_theEventSystem->FireEvent("KeyPressed", args);
			if (!wasConsumed)
			{
				wasConsumed = input->HandleKeyPressed(keyCode);
			}
			if (wasConsumed)
			{
				return 0;
			}
			break;
		}

		case WM_KEYUP:
		{
			unsigned char keyCode = static_cast<unsigned char>(wParam);
			bool wasConsumed = false;

			if (!wasConsumed)
			{
				wasConsumed = input->HandleKeyReleased(keyCode);
			}
			if (wasConsumed)
			{
				return 0;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			bool wasConsumed = false;

			wasConsumed = input->HandleKeyPressed(KEYCODE_LMB);

			if (wasConsumed)
			{
				return 0;
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			bool wasConsumed = false;

			wasConsumed = input->HandleKeyPressed(KEYCODE_RMB);
			if (wasConsumed)
			{
				return 0;
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			bool wasConsumed = false;

			wasConsumed = input->HandleKeyReleased(KEYCODE_LMB);
			if (wasConsumed)
			{
				return 0;
			}
			break;
		}

		case WM_RBUTTONUP:
		{
			bool wasConsumed = false;

			wasConsumed = input->HandleKeyReleased(KEYCODE_RMB);
			if (wasConsumed)
			{
				return 0;
			}
			break;
		}
	}
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

Window::Window(WindowConfig const& config)
	: m_config(config)
{
	if (s_theWindow == nullptr)
	{
		s_theWindow = this;
		m_config.m_inputSystem->m_window = this;
	}
	else
	{
		ASSERT_RECOVERABLE(s_theWindow != nullptr, "Attempted to create a new window when one already exists!");
	}
}

Vec3 Window::GetMouseForward(float degrees, Camera const& camera) const
{
	Vec2 cursorPos = GetNormalizedCursorPos();

	float x = cursorPos.x;
	float y = cursorPos.y;

	IntVec2 dimensions = GetClientDimensions();
	float screenHeight = (float)dimensions.y;
	float screenWidth = (float)dimensions.x;

	float planeDistance = screenHeight / (2.f * TanDegrees(degrees/2.f));

	Vec3 forward = camera.GetForward();
	Vec3 left = camera.GetLeft();
	Vec3 up = camera.GetUp();

	Vec3 center = camera.GetPosition() + forward * planeDistance;
	Vec3 rightOffset = left * (x - 0.5f) * screenWidth;
	Vec3 upOffset = up * (y - 0.5f) * screenHeight;

	Vec3 target = center + rightOffset + upOffset;

	return (target - camera.GetPosition()).GetNormalized();
}

void Window::Startup()
{
	CreateOSWindow();
	m_cursorHandle = LoadCursor(NULL, IDC_ARROW);
}

void Window::BeginFrame()
{
	if (m_openFileNextFrame)
	{
		OPENFILENAMEA ofn;       // common dialog box structure
		char szFile[1024];       // buffer for file name
		HWND hwnd = (HWND)GetHwnd();              // owner window

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = (LPSTR)szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = ".*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		// Display the Open dialog box. 
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			EventArgs args;
			args.SetValue("FilePath", (char const*)ofn.lpstrFile);

			g_theEventSystem->FireEvent("FileSelected", args);
		}

		m_openFileNextFrame = false;

	}
	DragAcceptFiles((HWND)m_windowHandle, TRUE);
	RunMessagePump();
}

void Window::EndFrame()
{
	DragAcceptFiles((HWND)m_windowHandle, FALSE);
	SetCursorIconImplementation();
}

void Window::Shutdown()
{

}


void Window::OpenFileNextFrame(char const* filePattern)
{
	m_nextFrameFilePattern = filePattern;
	m_openFileNextFrame = true;
}

void Window::SetCursorIcon(CursorIcon icon)
{
	m_cursorIcon = icon;
}

Vec2 Window::GetNormalizedCursorPos() const
{
	HWND windowHandle = HWND(m_windowHandle);
	POINT cursorCoords;
	RECT clientRect;
	GetCursorPos(&cursorCoords);
	ScreenToClient(windowHandle, &cursorCoords);
	GetClientRect(windowHandle, &clientRect);
	float cursorX = float(cursorCoords.x) / float(clientRect.right);
	float cursorY = float(cursorCoords.y) / float(clientRect.bottom);
	return Vec2(cursorX, 1.f - cursorY);
}

IntVec2 Window::GetClientDimensions() const
{
	return m_dimensions;
}

void* Window::GetHwnd() const
{
	return m_windowHandle;
}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}

float Window::GetAspect() const
{
	return m_config.m_aspectRatio;
}

void* Window::GetDisplayContext() const
{
	return m_displayContext;
}

Window* Window::GetMainWindowInstance()
{
	return s_theWindow;
}

void Window::SetCursorIconImplementation()
{
	LPCTSTR cursor = IDC_ARROW;
	switch (m_cursorIcon)
	{
		case CursorIcon::POINTER: cursor = IDC_ARROW; break;
		case CursorIcon::HAND: cursor = IDC_HAND; break;
	}
	m_cursorHandle = LoadCursor(NULL, cursor);
	SetCursor((HCURSOR)m_cursorHandle);
}

void Window::CreateOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;

	if (m_config.m_aspectRatio > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / m_config.m_aspectRatio;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * m_config.m_aspectRatio;
	}

	if (m_config.m_size != Vec2(-1, -1))
	{
		clientWidth = (float)m_config.m_size.x;
		clientHeight = (float)m_config.m_size.y;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	if (m_config.m_position != Vec2(-1, -1))
	{
		clientMarginX = m_config.m_position.x;
		clientMarginY = m_config.m_position.y;
	}

	if (m_config.m_fullscreen)
	{
		clientWidth = desktopWidth;
		clientHeight = desktopHeight;
		clientMarginX = 0;
		clientMarginY = 0;
	}

	RECT clientRect;

	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_windowHandle = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		NULL,
		NULL);
	m_dimensions = IntVec2((int)clientWidth,(int)clientHeight);
	ShowWindow((HWND)m_windowHandle, SW_SHOW);
	SetForegroundWindow((HWND)m_windowHandle);
	SetFocus((HWND)m_windowHandle);
	
	m_displayContext = GetDC((HWND)m_windowHandle);

	m_config.m_aspectRatio = (float)clientWidth / (float)clientHeight;
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}