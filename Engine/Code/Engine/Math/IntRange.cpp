#include "IntRange.hpp"

IntRange const IntRange::ZERO = IntRange(0, 0);
IntRange const IntRange::ZERO_TO_ONE = IntRange(0, 1);
IntRange const IntRange::ONE = IntRange(1, 1);

IntRange::IntRange()
	: m_min(0)
	, m_max(0)
{

}

IntRange::IntRange(int min, int max)
	: m_min(min)
	, m_max(max)
{
}

bool IntRange::IsOnRange(int value) const
{
	return value >= m_min && value <= m_max;
}

bool IntRange::IsOverlappingWith(IntRange const& otherRange) const
{
	return otherRange.IsOnRange(m_min) || otherRange.IsOnRange(m_max) || IsOnRange(otherRange.m_min) || IsOnRange(otherRange.m_max);
}

bool IntRange::operator==(IntRange const& otherRange)
{
	return m_max == otherRange.m_max && m_min == otherRange.m_min;
}

bool IntRange::operator!=(IntRange const& otherRange)
{
	return !(*this == otherRange);
}

void IntRange::operator=(IntRange const& otherRange)
{
	m_max = otherRange.m_max;
	m_min = otherRange.m_min;
}
