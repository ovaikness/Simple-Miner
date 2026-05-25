#pragma once
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <vector>

class CatmullRomSpline2D
{
public:
	CatmullRomSpline2D() = default;
	CatmullRomSpline2D(std::vector<Vec2> const& points);

	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 GetTangentAtParametric(float parametricZeroToOne) const;
	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	Vec2 GetTangentAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	size_t GetCurveCount() const;
	CubicBezierCurve2D const& GetCurve(size_t index) const;
protected:
	std::vector<CubicBezierCurve2D> m_curves;
};