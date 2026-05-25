#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <Windows.h>

unsigned char const KEYCODE_F1 = VK_F1;
unsigned char const KEYCODE_F2 = VK_F2;
unsigned char const KEYCODE_F3 = VK_F3;
unsigned char const KEYCODE_F4 = VK_F4;
unsigned char const KEYCODE_F5 = VK_F5;
unsigned char const KEYCODE_F6 = VK_F6;
unsigned char const KEYCODE_F7 = VK_F7;
unsigned char const KEYCODE_F8 = VK_F8;
unsigned char const KEYCODE_F9 = VK_F9;
unsigned char const KEYCODE_F10 = VK_F10;
unsigned char const KEYCODE_F11 = VK_F11;
unsigned char const KEYCODE_ESCAPE = VK_ESCAPE;
unsigned char const KEYCODE_UP = VK_UP;
unsigned char const KEYCODE_DOWN = VK_DOWN;
unsigned char const KEYCODE_LEFT = VK_LEFT;
unsigned char const KEYCODE_RIGHT = VK_RIGHT;
unsigned char const KEYCODE_SPACE = VK_SPACE;
unsigned char const KEYCODE_ENTER = VK_RETURN;
unsigned char const KEYCODE_RMB = VK_RBUTTON;
unsigned char const KEYCODE_LMB = VK_LBUTTON;
unsigned char const KEYCODE_TILDE = 0xc0;
unsigned char const KEYCODE_LEFTBRACKET = 0xDB;
unsigned char const KEYCODE_RIGHTBRACKET = 0xDD;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END = VK_END;
unsigned char const KEYCODE_SHIFT = VK_SHIFT;
unsigned char const KEYCODE_COMMA = VK_OEM_COMMA;
unsigned char const KEYCODE_PERIOD = VK_OEM_PERIOD;

void InputSystem::RegisterInputState(XboxController& controller, GamepadButtonID button, DWORD mapping, WORD wButtons)
{
	controller.m_buttons[button].m_wasPressedLastFrame = controller.m_buttons[button].m_isPressed;
	controller.m_buttons[button].m_isPressed = (wButtons & mapping) == mapping;
}

InputSystem::InputSystem(InputSystemConfig const& config)
	:m_config(config)
{
	m_controllers[0] = XboxController(0);
	m_controllers[1] = XboxController(1);
	m_controllers[2] = XboxController(2);
	m_controllers[3] = XboxController(3);
	float innerDeadzone = 0.3f;
	float outerDeadzone = 0.95f;
	for(unsigned int controllerIndex = 0; controllerIndex < 4; ++controllerIndex)
	{
		m_controllers[controllerIndex].m_rightStick.SetDeadZoneThresholds(innerDeadzone,outerDeadzone);
		m_controllers[controllerIndex].m_leftStick.SetDeadZoneThresholds(innerDeadzone, outerDeadzone);
	}
}

void InputSystem::BeginFrame()
{
	DWORD dwResult;
	for (unsigned int controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
	{
		XINPUT_STATE state;
		ZeroMemory( &state, sizeof(XINPUT_STATE) );
		dwResult = XInputGetState(controllerIndex, &state);
		XboxController& controller = m_controllers[controllerIndex];

		if (dwResult == ERROR_SUCCESS)
		{
			WORD wButtons = state.Gamepad.wButtons;
			controller.m_isConnected = true;
			RegisterInputState(controller, GAMEPAD_BUTTON_A,	 XINPUT_GAMEPAD_A, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_B,	 XINPUT_GAMEPAD_B, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_X,	 XINPUT_GAMEPAD_X, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_Y,	 XINPUT_GAMEPAD_Y, wButtons);

			RegisterInputState(controller, GAMEPAD_BUMPER_LEFT,	 XINPUT_GAMEPAD_LEFT_SHOULDER, wButtons);
			RegisterInputState(controller, GAMEPAD_BUMPER_RIGHT, XINPUT_GAMEPAD_RIGHT_SHOULDER, wButtons);

			RegisterInputState(controller, GAMEPAD_BUTTON_DOWN,	 XINPUT_GAMEPAD_DPAD_DOWN, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_UP,	 XINPUT_GAMEPAD_DPAD_UP, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_LEFT,	 XINPUT_GAMEPAD_DPAD_LEFT, wButtons);

			RegisterInputState(controller, GAMEPAD_BUTTON_BACK,	 XINPUT_GAMEPAD_BACK, wButtons);
			RegisterInputState(controller, GAMEPAD_BUTTON_START, XINPUT_GAMEPAD_START, wButtons);

			RegisterInputState(controller, GAMEPAD_RIGHT_STICK,	 XINPUT_GAMEPAD_RIGHT_THUMB, wButtons);
			RegisterInputState(controller, GAMEPAD_LEFT_STICK,	 XINPUT_GAMEPAD_LEFT_THUMB, wButtons);

			controller.m_leftStick.UpdatePosition(
				RangeMapClamped(static_cast<float>(state.Gamepad.sThumbLX),-32768.f, 32767.f, -1.f, 1.f),
				RangeMapClamped(static_cast<float>(state.Gamepad.sThumbLY),-32768.f, 32767.f, -1.f, 1.f)
			);

			controller.m_rightStick.UpdatePosition(
				RangeMapClamped(static_cast<float>(state.Gamepad.sThumbRX), -32768.f, 32767.f, -1.f, 1.f),
				RangeMapClamped(static_cast<float>(state.Gamepad.sThumbRY), -32768.f, 32767.f, -1.f, 1.f)
			);

			controller.m_leftTrigger = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.f;
			controller.m_rightTrigger = static_cast<float>(state.Gamepad.bRightTrigger) / 255.f;
			
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = static_cast<unsigned short>(controller.m_leftRumble * 65535.f);
			vibration.wRightMotorSpeed = static_cast<unsigned short>(controller.m_rightRumble * 65535.f);
			XInputSetState(controllerIndex, &vibration);
		}
		else
		{
			controller.m_isConnected = false;
		}
	}
	if (m_window == nullptr)
	{
		return;
	}

	bool hasFocus = m_window->GetHwnd() == GetActiveWindow();

	if (m_justRecievedFocus)
	{
		Vec2 normalizedCursorPos = GetCursorNormalizedPosition();
		IntVec2 dim = m_window->GetClientDimensions();
		Vec2 dimf = Vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
		Vec2 clientPos = dimf * normalizedCursorPos;
		m_cursorState.m_cursorClientPosition = IntVec2(static_cast<int>(clientPos.x), static_cast<int>(clientPos.y));

		IntVec2 screenCoordsCenter = dim / 2;
		IntVec2 cursorOnScreen;
		ClientToScreen((HWND)m_window->GetHwnd(), (LPPOINT)&screenCoordsCenter);
		GetCursorPos((LPPOINT)&cursorOnScreen);

		m_cursorState.m_cursorClientDelta = cursorOnScreen - screenCoordsCenter;

		SetCursorPos(screenCoordsCenter.x, screenCoordsCenter.y);

		IntVec2 actualPos;
		GetCursorPos((LPPOINT)&actualPos);
		m_cursorState.m_cursorClientPosition = actualPos;
		m_justRecievedFocus = false;
	}

	if (m_cursorState.m_relativeMode && hasFocus)
	{
		Vec2 normalizedCursorPos = GetCursorNormalizedPosition();
		IntVec2 dim = m_window->GetClientDimensions();
		Vec2 dimf = Vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
		Vec2 clientPos = dimf * normalizedCursorPos;
		m_cursorState.m_cursorClientPosition = IntVec2(static_cast<int>(clientPos.x), static_cast<int>(clientPos.y));

		IntVec2 screenCoordsCenter = dim / 2;
		IntVec2 cursorOnScreen;
		ClientToScreen((HWND)m_window->GetHwnd(), (LPPOINT)&screenCoordsCenter);
		GetCursorPos((LPPOINT)&cursorOnScreen);

		m_cursorState.m_cursorClientDelta = cursorOnScreen - screenCoordsCenter;

		SetCursorPos(screenCoordsCenter.x, screenCoordsCenter.y);

		IntVec2 actualPos;
		GetCursorPos((LPPOINT)&actualPos);
		m_cursorState.m_cursorClientPosition = actualPos;
	}
	else
	{
		Vec2 normalizedCursorPos = GetCursorNormalizedPosition();
		IntVec2 dim = m_window->GetClientDimensions();
		Vec2 dimf = Vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
		Vec2 clientPos = dimf * normalizedCursorPos;
		m_cursorState.m_cursorClientPosition = IntVec2(static_cast<int>(clientPos.x), static_cast<int>(clientPos.y));
		m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
	}

	if (m_cursorState.m_hidden && hasFocus)
	{
		int count;
		do
		{
			count = ShowCursor(false);
		} while (count >= 0);
	}
	else
	{
		int count;
		do 
		{
			count = ShowCursor(true);
		} while (count < 0);
	}
}

void InputSystem::EndFrame()
{
	for (unsigned int keyCode = 0; keyCode < 256; ++keyCode)
	{
		m_keysJustPressed[keyCode] = false;
		m_keysJustReleased[keyCode] = false;
	}

	m_scrollDelta = 0;
}

void InputSystem::SetCursorState(bool m_hidden, bool m_relativeMode)
{
	m_cursorState.m_hidden = m_hidden;
	m_cursorState.m_relativeMode = m_relativeMode;
}

IntVec2 InputSystem::GetCursorClientDelta() const
{
	return m_cursorState.m_cursorClientDelta;
}

Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	if (m_window)
	{
		return m_window->GetNormalizedCursorPos();
	}
	else
	{
		return Vec2(0.f, 0.f);
	}
}

int InputSystem::GetScrollDelta() const
{
	return m_scrollDelta;
}

void InputSystem::HandleFocusLost()
{
	for (unsigned int keyCode = 0; keyCode < 256; ++keyCode)
	{
		m_keysDown[keyCode] = false;
		m_keysJustPressed[keyCode] = false;
		m_keysJustReleased[keyCode] = false;
	}
}

void InputSystem::HandleFocusGained()
{
	m_justRecievedFocus = true;
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	return m_keysJustPressed[keyCode];
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	return m_keysJustReleased[keyCode];
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keysDown[keyCode];
}

XboxController* InputSystem::GetController(int id)
{
	return &m_controllers[id];
}

bool InputSystem::HandleScrollDelta(int delta)
{
	m_scrollDelta += delta;
	return false;
}

bool InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	if (!m_keysDown[keyCode])
	{
		m_keysDown[keyCode] = true;
		m_keysJustPressed[keyCode] = true;
	}
 	return false;
}

bool InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keysDown[keyCode] = false;
	m_keysJustReleased[keyCode] = true;
	return false;
}
