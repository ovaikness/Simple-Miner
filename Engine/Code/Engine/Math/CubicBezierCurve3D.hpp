#pragma once
#include "Engine/Math/Vec3.hpp"

class CubicBezierCurve3D
{
public:
	CubicBezierCurve3D() = default;
	CubicBezierCurve3D(Vec3 const& startPos, Vec3 const& guidePos1, Vec3 const& guidePos3, Vec3 const& endPos);

	void MakeHermite(Vec3 const& startPos, Vec3 const& startVelocity, Vec3 const& endPos, Vec3 const& endVelocity);
	Vec3 GetHermiteStartVelocity() const;
	Vec3 GetHermiteEndVelocity() const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec3 EvaluateAtParametric(float parametricZeroToOne) const;
	Vec3 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
public:
	Vec3 m_startPos = Vec3(0.f, 0.f, 0.f);
	Vec3 m_guidePos1 = Vec3(0.f, 0.f, 0.f);
	Vec3 m_guidePos3 = Vec3(0.f, 0.f, 0.f);
	Vec3 m_endPos = Vec3(0.f, 0.f, 0.f);
};