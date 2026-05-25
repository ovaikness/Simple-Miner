#pragma once
#include "Engine/Math/Vec3.hpp"
#include <vector>

struct AABB3
{
public:
	AABB3();
	AABB3(Vec3 const& mins, Vec3 const& maxs);
	AABB3(float minx, float miny, float minz, float maxx, float maxy, float maxz);
	Vec3 m_mins;
	Vec3 m_maxs;

	std::vector<Vec3> GetCorners() const;
	bool IsPointInside(Vec3 const& point) const;
	void Translate(Vec3 const& translation);
	Vec3 GetCenter() const;
	AABB3 GetTranslated(Vec3 const& translation);
protected:
};