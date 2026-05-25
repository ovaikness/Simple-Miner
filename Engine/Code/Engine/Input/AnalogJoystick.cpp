#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include <iostream>

Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadzone;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadzone;
}

void AnalogJoystick::Reset()
{
	m_correctedPosition = Vec2();
}

void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneTheshold, float normalizedOutDeadzoneThreshold)
{
	m_innerDeadzone = normalizedInnerDeadzoneTheshold;
	m_outerDeadzone = normalizedOutDeadzoneThreshold;
}

void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);
	m_magnitude = m_rawPosition.GetLength();
	m_polarDegrees = m_rawPosition.GetOrientationDegrees();
	m_magnitude = RangeMapClamped( m_magnitude, m_innerDeadzone, m_outerDeadzone, 0.f, 1.f);
	m_correctedPosition = Vec2::MakeFromPolarDegrees(m_polarDegrees, m_magnitude);
}
