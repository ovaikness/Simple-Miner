#pragma once
#include "Engine/Math/Vec2.hpp"

class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D() = default;
	CubicBezierCurve2D(Vec2 const& startPos, Vec2 const& guidePos1, Vec2 const& guidePos2, Vec2 const& endPos);
	
	void MakeHermite(Vec2 const& startPos, Vec2 const& startVelocity, Vec2 const& endPos, Vec2 const& endVelocity);
	Vec2 GetHermiteStartVelocity() const;
	Vec2 GetHermiteEndVelocity() const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 GetTangentAtParametric(float parametricZeroToOne) const;
	Vec2 GetTangentAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
public:
	Vec2 m_startPos  = Vec2(0.f,0.f);
	Vec2 m_guidePos1 = Vec2(0.f,0.f);
	Vec2 m_guidePos2 = Vec2(0.f,0.f);
	Vec2 m_endPos	 = Vec2(0.f,0.f);
};
