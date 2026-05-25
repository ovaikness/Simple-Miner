#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"

class OBB2
{
public:
	OBB2();
	OBB2(Vec2 const& min, Vec2 const& max, Vec2 const& iBasis);
	OBB2(Vec2 const& center, float width, float height, float degreesOrientation);
	OBB2(Vec2 const& center, float width, float height, Vec2 const& iBasis);
	OBB2(AABB2 const& bounds);
	Vec2 const GetIBasis() const;
	Vec2 const GetJBasis() const;

	void Translate(Vec2 const& translation);
	void SetIBasis(Vec2 const& iBasis);

	bool IsPointInside(Vec2 const& point) const;
	Vec2 const GetNearestPoint(Vec2 const& point) const;
	void SetCenter(Vec2 const& center);
	void SetRotation(float degrees, Vec2 const& origin = Vec2(0.5f, 0.5f));
	void Rotate(float degrees, Vec2 const& origin = Vec2(0.5f,0.5f));
	float GetOrientationDegrees() const;

	//Point Ref
	Vec2 const GetTopLeft() const;
	Vec2 const GetTopRight() const;
	Vec2 const GetBottomLeft() const;
	Vec2 const GetBottomRight() const;

	//AABB Conversions
	Vec2 const  GetMin() const;
	Vec2 const  GetMax() const;
	AABB2 const GetAABB() const;
public:
	float m_halfWidth = 1.f;
	float m_halfHeight = 1.f;
	Vec2  m_iBasis = Vec2(1.f, 0.f);
	Vec2  m_center = Vec2(0.f, 0.f);
};