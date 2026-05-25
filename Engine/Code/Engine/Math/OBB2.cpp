#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

OBB2::OBB2()
{
}

OBB2::OBB2(Vec2 const& min, Vec2 const& max, Vec2 const& iBasis)
{
	m_center = (min + max) * 0.5f;
	m_iBasis = iBasis.GetNormalized();

	Vec2 displacement = max - min;

	m_halfWidth = DotProduct2D(displacement, iBasis) * 0.5f;
	m_halfHeight = DotProduct2D(displacement, iBasis.GetRotated90Degrees()) * 0.5f;
}

OBB2::OBB2(Vec2 const& center, float width, float height, float degreesOrientation)
	: m_center(center)
	, m_halfWidth(width / 2.f)
	, m_halfHeight(height / 2.f)
	, m_iBasis(Vec2::MakeFromPolarDegrees(degreesOrientation, 1.f))
{
}

OBB2::OBB2(Vec2 const& center, float width, float height, Vec2 const& iBasis)
	: m_center(center)
	, m_halfWidth(width / 2.f)
	, m_halfHeight(height / 2.f)
	, m_iBasis(iBasis)
{
}

OBB2::OBB2(AABB2 const& bounds) : OBB2(bounds.m_min, bounds.m_max, Vec2(1.f,0.f))
{
}

Vec2 const OBB2::GetIBasis() const
{
	return m_iBasis;
}

Vec2 const OBB2::GetJBasis() const
{
	return m_iBasis.GetRotated90Degrees();
}

void OBB2::Translate(Vec2 const& translation)
{
	m_center += translation;
}

void OBB2::SetIBasis(Vec2 const& iBasis)
{
	m_iBasis = iBasis;
}

bool OBB2::IsPointInside(Vec2 const& point) const
{
	float pointI = DotProduct2D(m_iBasis, point - m_center);
	if (pointI > m_halfWidth || pointI < -m_halfWidth)
	{
		return false;
	}

	Vec2 jBasis = m_iBasis.GetRotated90Degrees();
	float pointJ = DotProduct2D(jBasis, point - m_center);
	if (pointJ > m_halfHeight || pointJ < -m_halfHeight)
	{
		return false;
	}
	return true;
}

Vec2 const OBB2::GetNearestPoint(Vec2 const& point) const
{
	Vec2 jBasis  = m_iBasis.GetRotated90Degrees();
	float pointJ = DotProduct2D(jBasis  , point - m_center);
	float pointI = DotProduct2D(m_iBasis, point - m_center);
	Vec2 pointIJ = Vec2(pointI, pointJ);
	if (pointIJ.x > m_halfWidth)
	{
		pointIJ.x = m_halfWidth;
	}
	else if (pointIJ.x < -m_halfWidth)
	{
		pointIJ.x = -m_halfWidth;
	}

	if (pointIJ.y > m_halfHeight)
	{
		pointIJ.y = m_halfHeight;
	}
	else if (pointIJ.y < -m_halfHeight)
	{
		pointIJ.y = -m_halfHeight;
	}

	TransformPosition2D(pointIJ, m_iBasis, jBasis, m_center);

	return pointIJ;
}

void OBB2::SetCenter(Vec2 const& center)
{
	m_center = center;
}

void OBB2::SetRotation(float degrees, Vec2 const& origin)
{
	Vec2 pivot;
	pivot.x = RangeMap(origin.x, 0.f, 1.f, -m_halfWidth, m_halfWidth) + m_center.x;
	pivot.y = RangeMap(origin.y, 0.f, 1.f, -m_halfHeight, m_halfHeight) + m_center.y;

	Vec2 translationToPivot = m_center - pivot;

	translationToPivot.RotateDegrees(degrees);

	m_center = pivot + translationToPivot;

	m_iBasis = Vec2::MakeFromPolarDegrees(degrees);
}

void OBB2::Rotate(float degrees, Vec2 const& origin)
{
	if (degrees == 0.f) return;
	Vec2 pivot;
	pivot.x = RangeMap(origin.x, 0.f, 1.f, -m_halfWidth , m_halfWidth) + m_center.x;
	pivot.y = RangeMap(origin.y, 0.f, 1.f, -m_halfHeight, m_halfHeight) + m_center.y;

	Vec2 translationToPivot = m_center - pivot;

	translationToPivot.RotateDegrees(degrees);

	m_center = pivot + translationToPivot;

	m_iBasis.RotateDegrees(degrees);
}

float OBB2::GetOrientationDegrees() const
{
	return m_iBasis.GetOrientationDegrees();
}

Vec2 const OBB2::GetTopLeft() const
{
	return m_center + (-m_iBasis * m_halfWidth) + (GetJBasis() * m_halfHeight);
}

Vec2 const OBB2::GetTopRight() const
{
	return m_center + (m_iBasis * m_halfWidth) + (GetJBasis() * m_halfHeight);
}

Vec2 const OBB2::GetBottomLeft() const
{
	return m_center + (-m_iBasis * m_halfWidth) + (-GetJBasis() * m_halfHeight);
}

Vec2 const OBB2::GetBottomRight() const
{
	return m_center + (m_iBasis * m_halfWidth) + (-GetJBasis() * m_halfHeight);
}

Vec2 const OBB2::GetMin() const
{
	return m_center - Vec2(m_halfWidth, m_halfHeight);
}

Vec2 const OBB2::GetMax() const
{
	return m_center + Vec2(m_halfWidth, m_halfHeight);
}

AABB2 const OBB2::GetAABB() const
{
	return AABB2(GetMin(), GetMax());
}
