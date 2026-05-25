#include "Engine/Math/ConvexHull2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/ConvexPoly2D.hpp"

#include <optional>

ConvexHull2D::ConvexHull2D(ConvexPoly2D const& poly)
	: ConvexHull2D(poly.GetPoints())
{
}

ConvexHull2D::ConvexHull2D(std::vector<Vec2> const& points)
{
	for (int i = 0; i < points.size(); i++)
	{
		Vec2 const& start = points[i];
		Vec2 const& end = points[(i + 1) % points.size()];
		Vec2 edge = end - start;
		Vec2 normal = edge.GetRotated90Degrees().GetNormalized();
		float distance = DotProduct2D(normal, start);
		m_planes.push_back({ normal,distance });
	}
}

ConvexHull2D::ConvexHull2D(std::vector<Vec2>&& points)
{
	for (int i = 0; i < points.size(); i++)
	{
		Vec2 const& start = points[i];
		Vec2 const& end = points[(i + 1) % points.size()];
		Vec2 edge = end - start;
		Vec2 normal = edge.GetRotated90Degrees().GetNormalized();
		float distance = DotProduct2D(normal, start);
		m_planes.push_back({ normal,distance });
	}
}

ConvexHull2D::~ConvexHull2D()
{

}

bool ConvexHull2D::IsPointInside(Vec2 const& point) const
{
	for (Plane2D const& plane : m_planes)
	{
		if (!plane.IsPointInFront(point))
		{
			return false;
		}
	}
	return true;
}

RaycastResult2D ConvexHull2D::Raycast(Vec2 const& start, Vec2 const& fwd, float maxDistance) const
{
	if (m_planes.empty())
	{
		return { false };
	}

	std::optional<RaycastResult2D> entryImpact;
	std::optional<RaycastResult2D> exitImpact;

	for (Plane2D const& plane : m_planes)
	{
		RaycastResult2D result = RaycastVsPlane2D(start, fwd, maxDistance, plane);
		if (!result.m_didImpact)
		{
			continue;
		}

		bool isEntry = DotProduct2D(plane.m_normal, fwd) > 0.f;

		if (isEntry)
		{
			if (!entryImpact || result.m_impactDist > entryImpact->m_impactDist)
			{
				entryImpact = result;
			}
		}
		else
		{
			if (!exitImpact || result.m_impactDist < exitImpact->m_impactDist)
			{
				exitImpact = result;
			}
		}
	}

	if (!entryImpact || !exitImpact)
	{
		Vec2 end = start + fwd * maxDistance;
		if (IsPointInside(end))
		{
			if (entryImpact)
			{
				return *entryImpact;
			}
		}
		if (IsPointInside(start))
		{
			RaycastResult2D result;
			result.m_didImpact = true;
			result.m_impactPos = start;
			result.m_impactDist = 0.f;
			result.m_impactNormal = -fwd;
			return result;
		}
		return { false };
	}

	// Midpoint check might be unnecessary
	Vec2 midPoint = (exitImpact->m_impactPos + entryImpact->m_impactPos) * 0.5f;

	if (IsPointInside(midPoint))
	{
		return *entryImpact;
	}

	return { false };
}
