#pragma once
#include "Engine/Math/CubicBezierCurve3D.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <vector>

class CatmullRomSpline3D
{
public:
	CatmullRomSpline3D() = default;
	CatmullRomSpline3D(std::vector<Vec3> const& points);

	float GetApproximateLength(int numSubdivisions = 64) const;

	Vec3 EvaluateAtParametric(float parametricZeroToOne) const;

	Vec3 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

	Vec3 GetNearestPoint(Vec3 const& point, int subDivisions = 64) const;

	size_t GetCurveCount() const;
	CubicBezierCurve3D const& GetCurve(size_t index) const;
protected:
	std::vector<CubicBezierCurve3D> m_curves;
};