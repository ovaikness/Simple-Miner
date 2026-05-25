#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec3.hpp"

struct RaycastResult2D
{
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;
};

struct RaycastResult3D
{
	bool m_didImpact = false;
	float m_impactDist = 0.f;
	Vec3 m_impactPos;
	Vec3 m_impactNormal;
	IntVec3 m_tileCoords;
};