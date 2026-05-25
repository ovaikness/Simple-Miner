#pragma once

class IntRange
{
public:
	static IntRange const ZERO;
	static IntRange const ZERO_TO_ONE;
	static IntRange const ONE;
	int m_min;
	int m_max;
public:
	IntRange();
	explicit IntRange(int min, int max);

	bool IsOnRange(int value) const;
	bool IsOverlappingWith(IntRange const& otherRange) const;

	void operator=(IntRange const& otherRange);
	bool operator==(IntRange const& otherRange);
	bool operator!=(IntRange const& otherRange);
};