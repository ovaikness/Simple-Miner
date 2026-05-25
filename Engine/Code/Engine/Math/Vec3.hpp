#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static Vec3 ZERO;		// (0,0,0)
public:
	// Construction/Destruction
	~Vec3() {}												// destructor (do nothing)
	Vec3() {}												// default constructor (do nothing)
	Vec3(const Vec3& copyFrom);							// copy constructor (from another vec2)
	explicit Vec3(Vec2 const&  copyFrom, float initialZ = 0.f);
	explicit Vec3(Vec2 const&& copyFrom, float initialZ = 0.f);
	explicit Vec3(float initialX, Vec2 const& copyFrom);
	explicit Vec3(float initialX, Vec2 const&& copyFrom);
	explicit Vec3(float initialX, float initialY, float initialZ);		// explicit constructor (from x, y)

	// Operators (const)
	bool		operator==(const Vec3& compare) const;		// vec2 == vec2
	bool		operator!=(const Vec3& compare) const;		// vec2 != vec2
	const Vec3	operator+(const Vec3& vecToAdd) const;		// vec2 + vec2
	const Vec3	operator-(const Vec3& vecToSubtract) const;	// vec2 - vec2
	const Vec3	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec3	operator*(float uniformScale) const;			// vec2 * float
	const Vec3	operator*(const Vec3& vecToMultiply) const;	// vec2 * vec2
	const Vec3	operator/(float inverseScale) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=(const Vec3& vecToAdd);				// vec2 += vec2
	void		operator-=(const Vec3& vecToSubtract);		// vec2 -= vec2
	void		operator*=(const float uniformScale);			// vec2 *= float
	void		operator/=(const float uniformDivisor);		// vec2 /= float
	void		operator=(const Vec3& copyFrom);				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec3 operator*(float uniformScale, const Vec3& vecToScale);	// float * vec2

	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZDegrees() const;
	float GetAngleAboutYDegrees() const;
	float GetAngleAboutZRadians() const;
	Vec3  GetRotatedAboutZDegrees(float degrees) const;
	Vec3  GetRotatedAboutZRadians(float radians) const;
	static Vec3  MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length = 1.f);
	Vec3  GetNormalized() const;

	void SetFromText(char const* text);

	void ClampLength(float  maxLength);
	void Normalize();
};


