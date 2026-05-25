#include "Engine/Math/CatmullRomSpline3D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

CatmullRomSpline3D::CatmullRomSpline3D(std::vector<Vec3> const& points)
{
	if (points.size() < 3)
	{
		ERROR_AND_DIE("Attempted to create CatmullRomSpline with less than 3 points!");
	}

	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		CubicBezierCurve3D curve;
		Vec3 startVel = Vec3(0.f, 0.f, 0.f);
		Vec3 endVel = Vec3(0.f, 0.f,0.f);
		if (i != 0)
		{
			startVel = (points[i + 1] - points[i - 1]) / 3.f;
		}
		if (i < points.size() - 3)
		{
			endVel = (points[i + 3] - points[i]) / 3.f;
		}

		curve.MakeHermite(points[i], startVel, points[i + 1], endVel);
		m_curves.push_back(curve);
	}
}

float CatmullRomSpline3D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	float accum = 0.f;
	for (CubicBezierCurve3D const& curve : m_curves)
	{
		accum += curve.GetApproximateLength(numSubdivisions);
	}
	return accum;
}

Vec3 CatmullRomSpline3D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float t = parametricZeroToOne * m_curves.size();

	for (size_t curveIndex = 0; curveIndex < m_curves.size(); ++curveIndex)
	{
		float tt = t - static_cast<float>(curveIndex);
		CubicBezierCurve3D const& curve = m_curves.at(curveIndex);
		if (abs(tt) < 0.00001f)
		{
			return curve.EvaluateAtParametric(0.f);
		}
		if (tt > 0.f && tt < 1.f)
		{
			return curve.EvaluateAtParametric(tt);
		}
	}

	return m_curves.at(m_curves.size() - 1).EvaluateAtParametric(1.f);
}

size_t CatmullRomSpline3D::GetCurveCount() const
{
	return m_curves.size();
}
CubicBezierCurve3D const& CatmullRomSpline3D::GetCurve(size_t index) const
{
	return m_curves.at(index);
}

Vec3 CatmullRomSpline3D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float accumDistance = 0.f;
	float curveDistance = 0.f;
	for (CubicBezierCurve3D const& curve : m_curves)
	{
		curveDistance = curve.GetApproximateLength(numSubdivisions);
		float distAppendCurve = accumDistance + curveDistance;
		if (distanceAlongCurve < distAppendCurve)
		{
			return curve.EvaluateAtApproximateDistance(distanceAlongCurve - accumDistance, numSubdivisions);
		}
		accumDistance += curveDistance;
	}
	return m_curves.at(m_curves.size() - 1).EvaluateAtParametric(1.f);
}

Vec3 CatmullRomSpline3D::GetNearestPoint(Vec3 const& point, int subDivisions /*= 64*/) const
{
	float closestDistanceSquared = 999999999999999999999.f;
	float parametricIncrement = 1.f / static_cast<float>(subDivisions);
	Vec3 lastPoint    = EvaluateAtParametric(0.f);
	Vec3 nextPoint    = EvaluateAtParametric(parametricIncrement);
	Vec3 closestPoint = GetNearestPointOnLineSegment3D(point,lastPoint,nextPoint);

	for (size_t i = 0; i <= subDivisions; ++i)
	{
		lastPoint = nextPoint;
		nextPoint = EvaluateAtParametric(static_cast<float>(i) * parametricIncrement);
		Vec3 currentPoint = GetNearestPointOnLineSegment3D(point,lastPoint,nextPoint);
		float currentDistanceSquared = GetDistanceSquared3D(point, currentPoint);
		if (currentDistanceSquared < closestDistanceSquared)
		{
			closestDistanceSquared = currentDistanceSquared;
			closestPoint = currentPoint;
		}
	}
	
	return closestPoint;
}