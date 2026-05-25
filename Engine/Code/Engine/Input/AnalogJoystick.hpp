#pragma once
#include "Engine/Math/Vec2.hpp"
class AnalogJoystick
{
public:
	AnalogJoystick() = default;

	Vec2 GetPosition() const;
	float GetMagnitude() const;
	float GetOrientationDegrees() const;

	Vec2 GetRawUncorrectedPosition() const;
	float GetInnerDeadZoneFraction() const;
	float GetOuterDeadZoneFraction() const;

	void Reset();
	void SetDeadZoneThresholds(float normalizedInnerDeadzoneTheshold, float normalizedOutDeadzoneThreshold);
	void UpdatePosition(float rawNormalizedX, float rawNormalizedY);
protected:
	Vec2 m_rawPosition;
	Vec2 m_correctedPosition;
	float m_polarDegrees  = 0.f;
	float m_magnitude	  =	0.f;
	float m_innerDeadzone = 0.f;
	float m_outerDeadzone = 1.f;
};