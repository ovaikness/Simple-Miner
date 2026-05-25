#include "Engine/Math/ConvexPoly2D.hpp"
#include "Engine/Math/ConvexHull2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <unordered_set>

namespace std
{
	template<>
	struct hash<Vec2>
	{
		std::size_t operator()(Vec2 const& v) const noexcept
		{
			std::size_t h1 = std::hash<float>{}(v.x);
			std::size_t h2 = std::hash<float>{}(v.y);
			return h1 ^ (h2 << 1);
		}
	};
}

ConvexPoly2D::ConvexPoly2D(std::vector<Vec2> const& points)
	: m_points(points)
{
}

ConvexPoly2D::ConvexPoly2D(std::vector<Vec2>&& points)
	: m_points(std::move(points))
{
}

ConvexPoly2D::~ConvexPoly2D()
{

}

std::vector<Vec2> const& ConvexPoly2D::GetPoints() const
{
	return m_points;
}

ConvexHull2D ConvexPoly2D::GetConvexHull() const
{
	return ConvexHull2D(m_points);
}

Vec2 ConvexPoly2D::GetCentroid() const
{
	Vec2 sum;
	for (Vec2 const& point : m_points)
	{
		sum += point;
	}
	return sum / static_cast<float>(m_points.size());
}

