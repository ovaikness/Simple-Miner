#include "FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"

FloatRange const FloatRange::ZERO		 = FloatRange(0.f,0.f);
FloatRange const FloatRange::ZERO_TO_ONE = FloatRange(0.f,1.f);
FloatRange const FloatRange::ONE		 = FloatRange(1.f,1.f);

FloatRange::FloatRange()
	: m_min(0.f)
	, m_max(0.f)
{

}

FloatRange::FloatRange(float min, float max)
	: m_min(min)
	, m_max(max)
{
}

bool FloatRange::IsOnRange(float value) const
{
	return value >= m_min && value <= m_max;
}

bool FloatRange::IsOverlappingWith(FloatRange const& otherRange) const
{
	return otherRange.IsOnRange(m_min) || otherRange.IsOnRange(m_max) || IsOnRange(otherRange.m_min) || IsOnRange(otherRange.m_max);
}

float FloatRange::GetIntersection(FloatRange const& otherRange) const
{
	if (IsOverlappingWith(otherRange))
	{
		if (otherRange.m_max > m_min)
		{
			return otherRange.m_max - m_min;
		}
		if (otherRange.m_min < m_max)
		{
			return m_max - otherRange.m_min;
		}
	}
	return 0.f;
}

float FloatRange::GetRandomOnRange(RandomNumberGenerator& rng) const
{
	return rng.RollRandomFloatInRange(m_min, m_max);
}

void FloatRange::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, '~');
	m_min = static_cast<float>(atof(strings.at(0).c_str()));
	m_max = static_cast<float>(atof(strings.at(1).c_str()));
}

bool FloatRange::operator==(FloatRange const& otherRange) const
{
	return m_max == otherRange.m_max && m_min == otherRange.m_min;
}

bool FloatRange::operator!=(FloatRange const& otherRange) const
{
	return !(*this == otherRange);
}

void FloatRange::operator=(FloatRange const& otherRange)
{
	m_max = otherRange.m_max;
	m_min = otherRange.m_min;
}
