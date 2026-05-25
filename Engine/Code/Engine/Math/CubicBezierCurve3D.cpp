#include "Engine/Math/CubicBezierCurve3D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicBezierCurve3D::CubicBezierCurve3D(Vec3 const& startPos, Vec3 const& guidePos1, Vec3 const& guidePos3, Vec3 const& endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos3(guidePos3)
	, m_endPos(endPos)
{
}

void CubicBezierCurve3D::MakeHermite(Vec3 const& startPos, Vec3 const& startVelocity, Vec3 const& endPos, Vec3 const& endVelocity)
{
	m_startPos = startPos;
	m_guidePos1 = startPos + startVelocity;
	m_guidePos3 = endPos - endVelocity;
	m_endPos = endPos;
}

Vec3 CubicBezierCurve3D::GetHermiteStartVelocity() const
{
	return m_guidePos1 - m_startPos;
}

Vec3 CubicBezierCurve3D::GetHermiteEndVelocity() const
{
	return m_endPos - m_guidePos3;
}


float CubicBezierCurve3D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	float distanceAccumulation = 0.f;
	float tInc = 1.f / numSubdivisions;
	for (int div = 0; div < numSubdivisions; ++div)
	{
		float t = div * tInc;
		float tNext = (div + 1) * tInc;

		Vec3 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos3, m_endPos, t);
		Vec3 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos3, m_endPos, tNext);

		distanceAccumulation += (second - first).GetLength();
	}

	return distanceAccumulation;
}

Vec3 CubicBezierCurve3D::EvaluateAtParametric(float parametricZeroToOne) const
{
	return ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos3, m_endPos, parametricZeroToOne);
}

Vec3 CubicBezierCurve3D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float distanceAccumulation = 0.f;
	float tInc = 1.f / numSubdivisions;
	float t = 0.f;
	float tNext = 0.f;
	for (int div = 0; div < numSubdivisions; ++div)
	{
		t = div * tInc;
		tNext = (div + 1) * tInc;

		Vec3 first = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos3, m_endPos, t);
		Vec3 second = ComputeCubicBezier(m_startPos, m_guidePos1, m_guidePos3, m_endPos, tNext);
		float segLength = (second - first).GetLength();
		if (distanceAccumulation + segLength > distanceAlongCurve)
		{
			float offset = distanceAlongCurve - distanceAccumulation;
			if (segLength == 0.f)
			{
				return first;
			}
			float tSeg = offset / segLength;
			return Interpolate(first, second, tSeg);
		}
		distanceAccumulation += segLength;
	}
	return m_endPos;
}

