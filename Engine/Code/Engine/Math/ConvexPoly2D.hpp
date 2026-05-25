#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RaycastResult.hpp"

#include <vector>

class ConvexHull2D;

class ConvexPoly2D
{
public:
	ConvexPoly2D(std::vector<Vec2> const& points);
	ConvexPoly2D(std::vector<Vec2>&& points);
	~ConvexPoly2D();

	std::vector<Vec2> const& GetPoints() const;
	ConvexHull2D GetConvexHull() const;
	Vec2 GetCentroid() const;
private:
	std::vector<Vec2> m_points;
};