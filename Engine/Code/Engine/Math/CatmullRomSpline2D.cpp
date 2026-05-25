#include "Engine/Math/CatmullRomSpline2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

CatmullRomSpline2D::CatmullRomSpline2D(std::vector<Vec2> const& points)
{
	if (points.size() < 2)
	{
		ERROR_AND_DIE("Attempted to create CatmullRomSpline with less than 2 points!");
	}

	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		CubicBezierCurve2D curve;
		Vec2 startVel = Vec2(0.f, 0.f);
		Vec2 endVel = Vec2(0.f, 0.f);
		if(i != 0)
		{
			startVel = (points[i + 1] - points[i - 1]) / 2.f;
		}
		if (i < points.size() - 2)
		{
			endVel   = (points[i + 2] - points[i]) / 2.f;
		}

		curve.MakeHermite(points[i], startVel, points[i + 1], endVel);
		m_curves.push_back(curve);
	}
}

float CatmullRomSpline2D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	float accum = 0.f;
	for (CubicBezierCurve2D const& curve : m_curves)
	{
		accum += curve.GetApproximateLength(numSubdivisions);
	}
	return accum;
}

Vec2 CatmullRomSpline2D::GetTangentAtParametric(float parametricZeroToOne) const
{
	float t = parametricZeroToOne * m_curves.size();

	for (size_t curveIndex = 0; curveIndex < m_curves.size(); ++curveIndex)
	{
		float tt = t - static_cast<float>(curveIndex);
		CubicBezierCurve2D const& curve = m_curves.at(curveIndex);
		if (abs(tt) < 0.00001f)
		{
			return curve.GetTangentAtParametric(0.f);
		}
		if (tt > 0.f && tt < 1.f)
		{
			return curve.GetTangentAtParametric(tt);
		}
	}

	return m_curves.at(m_curves.size() - 1).GetTangentAtParametric(1.f);
}

Vec2 CatmullRomSpline2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float t = parametricZeroToOne * m_curves.size();

	for (size_t curveIndex = 0; curveIndex < m_curves.size(); ++curveIndex)
	{
		float tt = t - static_cast<float>(curveIndex);
		CubicBezierCurve2D const& curve = m_curves.at(curveIndex);
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

size_t CatmullRomSpline2D::GetCurveCount() const
{
	return m_curves.size();
}
CubicBezierCurve2D const& CatmullRomSpline2D::GetCurve(size_t index) const
{
	return m_curves.at(index);
}

Vec2 CatmullRomSpline2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float accumDistance = 0.f;
	float curveDistance = 0.f;
	for (CubicBezierCurve2D const& curve : m_curves)
	{
		curveDistance = curve.GetApproximateLength(numSubdivisions);
		float distAppendCurve = accumDistance + curveDistance;
		if (distanceAlongCurve < distAppendCurve)
		{
			return curve.EvaluateAtApproximateDistance(distanceAlongCurve - accumDistance, numSubdivisions);
		}
		accumDistance += curveDistance;
	}
	return m_curves.at(m_curves.size()-1).EvaluateAtParametric(1.f);
}

Vec2 CatmullRomSpline2D::GetTangentAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float accumDistance = 0.f;
	float curveDistance = 0.f;
	for (CubicBezierCurve2D const& curve : m_curves)
	{
		curveDistance = curve.GetApproximateLength(numSubdivisions);
		float distAppendCurve = accumDistance + curveDistance;
		if (distanceAlongCurve < distAppendCurve)
		{
			return curve.GetTangentAtApproximateDistance(distanceAlongCurve - accumDistance, numSubdivisions);
		}
		accumDistance += curveDistance;
	}
	return m_curves.at(m_curves.size() - 1).GetTangentAtParametric(1.f);
}