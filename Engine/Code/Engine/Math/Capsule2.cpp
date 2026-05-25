#include "Engine/Math/Capsule2.hpp"

Capsule2::Capsule2()
	: m_bone(LineSegment2(Vec2(0.f,0.f), Vec2(0.f,0.f)))
	, m_radius(0.f)
{
}

Capsule2::Capsule2(Vec2 const& begin, Vec2 const& end, float radius)
	: m_bone(LineSegment2(begin,end))
	, m_radius(radius)
{
}
