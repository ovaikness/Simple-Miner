#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/INput/AnalogJoystick.hpp"
#pragma comment( lib, "xinput9_1_0" )
#include <windows.h>
#include <Xinput.h>

enum GamepadButtonID
{
	GAMEPAD_BUTTON_A,
	GAMEPAD_BUTTON_B,
	GAMEPAD_BUTTON_X,
	GAMEPAD_BUTTON_Y,
	GAMEPAD_BUTTON_DOWN,
	GAMEPAD_BUTTON_UP,
	GAMEPAD_BUTTON_LEFT,
	GAMEPAD_BUTTON_RIGHT,
	GAMEPAD_BUTTON_START,
	GAMEPAD_BUTTON_BACK,
	GAMEPAD_BUMPER_LEFT,
	GAMEPAD_BUMPER_RIGHT,
	GAMEPAD_LEFT_STICK,
	GAMEPAD_RIGHT_STICK
};
class InputSystem;
class XboxController
{
public:
	XboxController() = default;
	XboxController(unsigned int id);
	bool				  IsConnected() const;
	int					  GetControllerID() const;
	AnalogJoystick const& GetLeftStick() const;
	AnalogJoystick const& GetRightStick() const;
	float				  GetLeftTrigger() const;
	float				  GetRightTrigger() const;
	void				  SetRightRumble(float magnitude);
	void				  SetLeftRumble(float magnitude);
	KeyButtonState const& GetButton(GamepadButtonID buttonID) const;
	bool				  IsButtonDown(GamepadButtonID buttonID) const;
	bool				  WasButtonJustPressed(GamepadButtonID buttonID) const;
	bool				  WasButtonJustReleased(GamepadButtonID buttonID) const;
protected:
	friend class InputSystem;
	bool m_isConnected = false;
	unsigned int m_id = 0;
	KeyButtonState m_buttons[14]{ false };
	AnalogJoystick m_leftStick;
	AnalogJoystick m_rightStick;
	float  m_rightTrigger = 0.f;
	float  m_leftTrigger = 0.f;

	float m_rightRumble = 0.f;
	float m_leftRumble  = 0.f;
};