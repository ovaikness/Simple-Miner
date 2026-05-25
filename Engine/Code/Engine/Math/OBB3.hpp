#pragma once

#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/RaycastResult.hpp"

#include <array>

class OBB3
{
public:
	Vec3  m_center = Vec3(0.f, 0.f, 0.f);
	float m_halfWidth = 1.f;
	float m_halfLength = 1.f;
	float m_halfHeight = 1.f;
public:
	OBB3() = default;
	explicit OBB3(AABB3 const& copy);
	OBB3(Vec3 center, float width, float height, float depth, EulerAngles orientation);

	RaycastResult3D GetRaycastResult(Vec3 const& position, Vec3 const& fwdNormal, float distance);

	void Translate(Vec3 const& translation);

	bool IntersectsZCylinder( Vec2 const& base, FloatRange heightRange, float radius);
	bool IntersectsSphere(Vec3 const& center, float radius);
	bool IntersectsOBB3(OBB3 const& obb3);
	bool IntersectsABB3(AABB3 const& abb3);

	bool IsPointInside(Vec3 const& point) const;
	Vec3 const GetNearestPoint(Vec3 const& point) const;

	void SetCenter(Vec3 const& center);

	void SetRotation(EulerAngles const& angles);
	void Rotate(EulerAngles const& angles);

	EulerAngles GetOrientationDegrees() const;

	std::array<Vec3,8> GetCorners() const;

	Vec3 const GetIBasis() const;
	Vec3 const GetJBasis() const;
	Vec3 const GetKBasis() const;
protected:
	bool OBB3VsOBB3IntersectionTest(OBB3 const& obb3) const;

	Vec3 m_iBasis = Vec3(1.f,0.f,0.f);
	Vec3 m_jBasis = Vec3(0.f,1.f,0.f);
	Vec3 m_kBasis = Vec3(0.f,0.f,1.f);
};