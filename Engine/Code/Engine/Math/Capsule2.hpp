#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/LineSegment2.hpp"

class Capsule2
{
public:
	LineSegment2 m_bone;
	float		 m_radius;
public:
	Capsule2();
	explicit Capsule2(Vec2 const& begin, Vec2 const& end, float radius);
};