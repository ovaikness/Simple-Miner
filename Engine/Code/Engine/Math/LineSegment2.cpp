#include "LineSegment2.hpp"

LineSegment2::LineSegment2()
	: m_start(Vec2(0.f,0.f))
	, m_end(Vec2(0.f,0.f))
{
}

LineSegment2::LineSegment2(Vec2 const& start, Vec2 const& end)
	: m_start(start)
	, m_end(end)
{
}
