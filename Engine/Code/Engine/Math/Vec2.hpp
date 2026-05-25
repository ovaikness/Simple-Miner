#pragma once
//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;

	static Vec2 ZERO;
	static Vec2 ONE;
public:
	// Construction/Destruction
	~Vec2() {};												// destructor (do nothing)
	Vec2() {};												// default constructor (do nothing)
	Vec2( const Vec2& copyFrom );							// copy constructor (from another vec2)
	explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)

	// Operators (const)
	bool		operator==( const Vec2& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec2& compare ) const;		// vec2 != vec2
	const Vec2	operator+( const Vec2& vecToAdd ) const;		// vec2 + vec2
	const Vec2	operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec2	operator*( float uniformScale ) const;			// vec2 * float
	const Vec2	operator*( const Vec2& vecToMultiply ) const;	// vec2 * vec2
	const Vec2	operator/( float inverseScale ) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=( const Vec2& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec2& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec2& copyFrom );				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2
	static Vec2 MakeFromPolarDegrees(float degrees, float length = 1);
	static Vec2 MakeFromPolarRadians(float radians, float length = 1);
	
	float GetOrientationDegrees() const;
	float GetOrientationRadians() const;
	float GetLength() const;
	float GetLengthSquared() const;

	void SetFromText(char const* text);
	Vec2 GetRotated90Degrees() const;
	Vec2 GetRotatedMinus90Degrees() const;
	Vec2 GetRotatedRadians(float radians) const;
	Vec2 GetRotatedDegrees(float degrees) const;
	void SetOrientationRadians(float radians);
	void SetOrientationDegrees(float degrees);
	void SetPolarRadians(float radians, float magnitude);
	void SetPolarDegrees(float degrees, float magnitude);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
	Vec2 GetClamped(float max) const;
	Vec2 GetNormalized() const;
	void SetLength(float length);
	void ClampLength(float max);
	float NormalizeAndGetPreviousLength();
	void Normalize();
	void Reflect(Vec2 const& normal);
	Vec2 GetReflected(Vec2 const& normal) const;
};
