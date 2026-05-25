#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"

bool Plane2D::IsPointInFront(Vec2 const& point) const
{
	float dotProduct = DotProduct2D(m_normal, point);
	if (dotProduct > m_distance)
	{
		return true;
	}
	return false;
}

Vec2 Plane2D::ProjectPointOntoPlane(Vec2 const& point) const
{
	float distance = GetSignedDistance(point);
	Vec2 projectedPoint = point - (m_normal * distance);
	return projectedPoint;
}

float Plane2D::GetSignedDistance(Vec2 const& point) const
{
	float distance = DotProduct2D(m_normal, point) - m_distance;
	return distance;
}
