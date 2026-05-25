#pragma once

#include "Engine/Math/CatmullRomSpline3D.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec3.hpp"

class VoxelCave
{
public:
	IntVec3 m_startPos;
	uint32_t m_seed;
	Vec3  m_start;
	bool  m_valid = false;
	float m_radius		= 0.f;
	float m_caveRadius	= 0.f;
	std::vector<Vec3> m_segs;
	std::vector<Vec3> m_backSegs;
public:
	VoxelCave(IntVec3 const& startPos, float caveRadius, float maxRadius, uint32_t seed);

	void Generate();
	bool IsBlockInside(IntVec3 const& point) const;
	bool IsBlockInside(IntVec3 const& point, float radius) const;
	bool IsABB3Inside(AABB3 const& bounds) const;
};

