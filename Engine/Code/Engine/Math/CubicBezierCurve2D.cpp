#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicBezierCurve2D::CubicBezierCurve2D(Vec2 const& startPos, Vec2 const& guidePos1, Vec2 const& guidePos2, Vec2 const& endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{
}

void CubicBezierCurve2D::MakeHermite(Vec2 const& startPos, Vec2 const& startVelocity, Vec2 const& endPos, Vec2 const& endVelocity)
{
	m_startPos  = startPos;
	m_guidePos1 = startPos + startVelocity;
	m_guidePos2 = endPos - endVelocity;
	m_endPos    = endPos;
}

Vec2 CubicBezierCurve2D::GetHermiteStartVelocity() const
{
	return m_guidePos1 - m_startPos;
}

Vec2 CubicBezierCurve2D::GetHermiteEndVelocity() const
{
	return m_endPos - m_guidePos2;
}


float CubicBezierCurve2D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	float distanceAccumulation = 0.f;
	float tInc = 1.f / numSubdivisions;
	for (int div = 0; div < numSubdivisions; ++div)
	{
		float t = div * tInc;
		float tNext = (div + 1) * tInc;

		Vec2 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, t);
		Vec2 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, tNext);

		distanceAccumulation += (second - first).GetLength();
	}

	return distanceAccumulation;
}

Vec2 CubicBezierCurve2D::GetTangentAtParametric(float parametricZeroToOne) const
{
	float t = parametricZeroToOne;
	float epsilon = 0.0001f;
	float tt = parametricZeroToOne + epsilon;
	if (tt > 1.f)
	{
		t  = 1.f - epsilon;
		tt = 1.f;
	}

	Vec2 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, t);
	Vec2 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, tt);

	Vec2 tangent = second - first;
	tangent.Normalize();

	return tangent;
}

Vec2 CubicBezierCurve2D::GetTangentAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	float distanceAccumulation = 0.f;
	float tInc = 1.f / numSubdivisions;
	float t = 0.f;
	float tNext = 0.f;
	float epsilon = 0.0001f;
	for (int div = 0; div < numSubdivisions; ++div)
	{
		t = div * tInc;
		tNext = (div + 1) * tInc;

		Vec2 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, t);
		Vec2 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, tNext);
		float segLength = (second - first).GetLength();
		if (distanceAccumulation + segLength > distanceAlongCurve)
		{
			return (second - first).GetNormalized();
		}
		distanceAccumulation += segLength;
	}

	Vec2 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, 1.f - epsilon);
	Vec2 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, 1.f);

	return (second - first).GetNormalized();
}

Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	return ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, parametricZeroToOne);
}

Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float distanceAccumulation = 0.f;
	float tInc = 1.f / numSubdivisions;
	float t = 0.f;
	float tNext = 0.f;
	for (int div = 0; div < numSubdivisions; ++div)
	{
		t = div * tInc;
		tNext = (div + 1) * tInc;

		Vec2 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, t);
		Vec2 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos2, m_endPos, tNext);
		float segLength = (second - first).GetLength();
		if (distanceAccumulation + segLength > distanceAlongCurve)
		{
			float offset = distanceAlongCurve - distanceAccumulation;
			if (segLength == 0.f)
			{
				return first;
			}
			float tSeg = offset / segLength;
			return Interpolate(first,second, tSeg);
		}
		distanceAccumulation += segLength;
	}
	return m_endPos;
}

