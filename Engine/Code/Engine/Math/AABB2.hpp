#pragma once
#include "Engine/Math/Vec2.hpp"

struct AABB2
{
public:
	Vec2 m_min;
	Vec2 m_max;
public:
	~AABB2();
	AABB2();
	AABB2(AABB2 const& copyFrom);
	explicit AABB2(float minx, float miny, float maxx, float maxy);
	explicit AABB2(Vec2 const& min, Vec2 const& max);

	bool IsPointInside(Vec2 const& point) const;
	Vec2 const GetCenter() const;
	Vec2 const GetDimensions() const;
	Vec2 const GetNearestPoint(Vec2 const& position) const;
	Vec2 const GetPointAtUV(Vec2 const& uv) const;
	Vec2 const GetUVForPoint(Vec2 const& position) const;

	float GetAspect() const;
	float GetWidth() const;
	float GetHeight() const;

	void Pad(float verticalPadding, float horizontalPadding);
	void Translate(Vec2 const& translation);
	AABB2 GetTranslated(Vec2 const& translation) const;

	Vec2 GetTopLeft() const;
	Vec2 GetTopRight() const;
	Vec2 GetBottomLeft() const;
	Vec2 GetBottomRight() const;

	void SetTopLeft(Vec2 const& topLeftCoords);
	void SetTopRight(Vec2 const& topRightCoords);
	void SetBottomLeft(Vec2 const& bottomLeftCoords);
	void SetBottomRight(Vec2 const& bottomRightCoords);

	void SetCenter(Vec2 const& center);
	void SetDimensions(Vec2 const& dimensions);
	void StretchToIncludePoint(Vec2 const& point);
private:

};