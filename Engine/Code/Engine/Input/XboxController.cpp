#include "Engine/Input/XboxController.hpp"

XboxController::XboxController(unsigned int id)
{
    m_id = id;
}

bool XboxController::IsConnected() const
{
    return m_isConnected;
}

int XboxController::GetControllerID() const
{
    return static_cast<unsigned int>(m_id);
}

AnalogJoystick const& XboxController::GetLeftStick() const
{
    return m_leftStick;
}

AnalogJoystick const& XboxController::GetRightStick() const
{
    return m_rightStick;
}

float XboxController::GetLeftTrigger() const
{
    return m_leftTrigger;
}

float XboxController::GetRightTrigger() const
{
    return m_rightTrigger;
}

void XboxController::SetRightRumble(float magnitude)
{
	m_rightRumble = magnitude;
}

void XboxController::SetLeftRumble(float magnitude)
{
	m_leftRumble = magnitude;
}

KeyButtonState const& XboxController::GetButton(GamepadButtonID buttonID) const
{
    // TODO: insert return statement here
    return m_buttons[buttonID];
}

bool XboxController::IsButtonDown(GamepadButtonID buttonID) const
{
	if (!IsConnected())
	{
		return false;
	}
    return m_buttons[buttonID].m_isPressed;
}

bool XboxController::WasButtonJustPressed(GamepadButtonID buttonID) const
{
	if (!IsConnected())
	{
		return false;
	}
    return m_buttons[buttonID].m_isPressed && !m_buttons[buttonID].m_wasPressedLastFrame;
}

bool XboxController::WasButtonJustReleased(GamepadButtonID buttonID) const
{
	if (!IsConnected())
	{
		return false;
	}
    return !m_buttons[buttonID].m_isPressed && m_buttons[buttonID].m_wasPressedLastFrame;
}
