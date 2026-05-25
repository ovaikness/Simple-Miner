#pragma once

#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/RaycastResult.hpp"

#include <vector>

class ConvexPoly2D;
class ConvexHull2D
{
public:
	ConvexHull2D(ConvexPoly2D const& poly);
	ConvexHull2D(std::vector<Vec2> const& points);
	ConvexHull2D(std::vector<Vec2>&& points);
	~ConvexHull2D();
	bool IsPointInside(Vec2 const& point) const;
	RaycastResult2D Raycast(Vec2 const& start, Vec2 const& fwd, float maxDistance) const;

	std::vector<Plane2D> m_planes;
};