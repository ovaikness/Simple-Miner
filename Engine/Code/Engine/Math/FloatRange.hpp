#pragma once
#include "Engine/Math/RandomNumberGenerator.hpp"
class FloatRange
{
public:
	static FloatRange const ZERO;
	static FloatRange const ZERO_TO_ONE;
	static FloatRange const ONE;
	float m_min;
	float m_max;
public:
	FloatRange();
	explicit FloatRange(float min, float max);

	bool IsOnRange(float value) const;
	bool IsOverlappingWith(FloatRange const& otherRange) const;
	float GetIntersection(FloatRange const& otherRange) const;
	float GetRandomOnRange(RandomNumberGenerator& rng) const;
	void SetFromText(char const* text);

	void operator=(FloatRange const& otherRange);
	bool operator==(FloatRange const& otherRange) const;
	bool operator!=(FloatRange const& otherRange) const;
};