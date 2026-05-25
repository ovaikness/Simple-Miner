#pragma once
#include "Vec2.hpp"

class LineSegment2
{
public:
	Vec2 m_start;
	Vec2 m_end;
public:
	LineSegment2();
	explicit LineSegment2(Vec2 const& start, Vec2 const& end);
};