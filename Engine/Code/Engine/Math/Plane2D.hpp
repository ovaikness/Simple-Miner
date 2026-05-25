#pragma once

#include "Engine/Math/Vec2.hpp"

class Plane2D
{

public:
	Vec2 m_normal;
	float m_distance = 0.f;

public:
	bool IsPointInFront(Vec2 const& point) const;

	Vec2  ProjectPointOntoPlane(Vec2 const& point) const;
	float GetSignedDistance(Vec2 const& point) const;
};