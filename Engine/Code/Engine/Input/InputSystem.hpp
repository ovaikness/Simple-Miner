#pragma once
#include "XboxController.hpp"
#include "Engine/Window/Window.hpp"

extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_ESCAPE;
extern unsigned char const KEYCODE_UP;
extern unsigned char const KEYCODE_DOWN;
extern unsigned char const KEYCODE_LEFT;
extern unsigned char const KEYCODE_RIGHT;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_LMB;
extern unsigned char const KEYCODE_RMB;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_LEFTBRACKET;
extern unsigned char const KEYCODE_RIGHTBRACKET;
extern unsigned char const KEYCODE_BACKSPACE;
extern unsigned char const KEYCODE_CLEAR;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;
extern unsigned char const KEYCODE_SHIFT;
extern unsigned char const KEYCODE_COMMA;
extern unsigned char const KEYCODE_PERIOD;

struct CursorState
{
	IntVec2 m_cursorClientDelta;
	IntVec2 m_cursorClientPosition;

	bool m_hidden = false;
	bool m_relativeMode = false;
};

struct InputSystemConfig
{
};

class InputSystem
{
public:
	XboxController m_controllers[4]		{};
	bool m_justRecievedFocus			{ false };
	bool m_wasPressedLastFrame[256]		{ false };
	bool m_keysJustPressed[256]			{ false };
	bool m_keysJustReleased[256]		{ false };
	bool m_keysDown[256]				{ false };
	int  m_scrollDelta = 0;

	CursorState m_cursorState {};
	InputSystemConfig m_config;
	Window* m_window = nullptr;
public:
	InputSystem(InputSystemConfig const& config = {});
	void BeginFrame();
	void EndFrame();

	void SetCursorState(bool m_hidden, bool m_relativeMode);
	IntVec2 GetCursorClientDelta() const;
	Vec2 GetCursorNormalizedPosition() const;
	int  GetScrollDelta() const;

	void HandleFocusLost();
	void HandleFocusGained();
	bool HandleScrollDelta(int delta);
	bool HandleKeyPressed(unsigned char keyCode);
	bool HandleKeyReleased(unsigned char keyCode);
	bool WasKeyJustPressed(unsigned char keyCode);
	bool WasKeyJustReleased(unsigned char keyCode);
	bool IsKeyDown(unsigned char keyCode);
	XboxController* GetController(int id);
	void RegisterInputState(XboxController& controller, GamepadButtonID button, DWORD mapping, WORD state);
};
