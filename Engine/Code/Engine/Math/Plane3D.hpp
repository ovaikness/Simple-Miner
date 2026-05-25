#pragma once
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
class Plane3D
{
public:
	Vec3 m_normal = Vec3(0.f, 0.f, 1.f);
	float m_distance = 0.f;
public:
	~Plane3D();
	Plane3D(Vec3 normal, float distance = 0.f);

	void	Normalize();
	EulerAngles GetOrientation() const;
	Plane3D GetNormalized() const;
	float	MagnitudeSquared() const;
	bool    IsPointInFrontOf(Vec3 const& point) const;
	Vec3	ProjectPointOntoPlane(Vec3 const& point) const;
	Vec3	GetTangentZUp() const;
	Vec3	GetBitangentZUp() const;
};