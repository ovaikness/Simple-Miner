#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

//#include "Engine/Core/EngineCommon.hpp"
#include <cmath>

Vec2 Vec2::ONE  = Vec2(1.f, 1.f);
Vec2 Vec2::ZERO = Vec2(0.f, 0.f);

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return compare.x == x && compare.y == y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return compare.x != x || compare.y != y;
}

void Vec2::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	x = static_cast<float>(atof(strings.at(0).c_str()));
	y = static_cast<float>(atof(strings.at(1).c_str()));
}

float Vec2::GetOrientationDegrees() const
{
	return Atan2Degrees(y,x);
}

float Vec2::GetOrientationRadians() const
{
	return atan2f(y,x);
}

float Vec2::GetLength() const
{
	return sqrtf(GetLengthSquared());
}

float Vec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2(-y,x);
}

Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2(y,-x);
}

Vec2 Vec2::GetRotatedRadians(float radians) const
{
	float transformedRadians = GetOrientationRadians() + radians;
	float length = GetLength();
	return Vec2(cosf(transformedRadians) * length, sinf(transformedRadians) * length);
}

Vec2 Vec2::GetRotatedDegrees(float degrees) const
{
	float transformedDegrees = GetOrientationDegrees() + degrees;
	float length = GetLength();
	return Vec2(CosDegrees(transformedDegrees) * length, SinDegrees(transformedDegrees) * length);
}

void Vec2::SetOrientationRadians(float radians)
{
	float length = GetLength();
	x = cosf(radians) * length;
	y = sinf(radians) * length;
}

void Vec2::SetOrientationDegrees(float degrees)
{
	float length = GetLength();
	x = CosDegrees(degrees) * length;
	y = SinDegrees(degrees) * length;
}

Vec2 Vec2::MakeFromPolarDegrees(float degrees, float length)
{
	return Vec2(CosDegrees(degrees) * length, SinDegrees(degrees) * length);
}

Vec2 Vec2::MakeFromPolarRadians(float radians, float length)
{
	return Vec2(cosf(radians) * length, sinf(radians) * length);
}

void Vec2::SetPolarRadians(float radians, float magnitude)
{
	x = cosf(radians) * magnitude;
	y = sinf(radians) * magnitude;
}

void Vec2::SetPolarDegrees(float degrees, float magnitude)
{
	x = CosDegrees(degrees) * magnitude;
	y = SinDegrees(degrees) * magnitude;
}

void Vec2::Rotate90Degrees()
{
	float temp = -y;
	y = x;
	x = temp;
}

void Vec2::RotateMinus90Degrees()
{
	float temp = -x;
	x = y;
	y = temp;
}

void Vec2::RotateDegrees(float degrees)
{
	float changedDegrees = Atan2Degrees(y, x) + degrees;
	float length = GetLength();
	x = CosDegrees(changedDegrees) * length;
	y = SinDegrees(changedDegrees) * length;
}

void Vec2::RotateRadians(float radians)
{
	float changedRadians = atan2f(y, x) + radians;
	float length = GetLength();
	x = cosf(changedRadians) * length;
	y = sinf(changedRadians) * length;
}

Vec2 Vec2::GetClamped(float max) const
{
	Vec2 copy = *this;
	copy.ClampLength(max);
	return copy;
}

Vec2 Vec2::GetNormalized() const
{
	Vec2 copy = *this;
	copy.Normalize();
	return copy;
}

void Vec2::SetLength(float length)
{
	float oldLength = GetLength();
	if (oldLength == 0.f) return;
	float scale = length / oldLength;
	x *= scale;
	y *= scale;
}

void Vec2::ClampLength(float maxLength)
{
	float oldLength = GetLength();
	if (oldLength > maxLength)
	{
		float scale = maxLength / oldLength;
		x *= scale;
		y *= scale;
	}
}

void Vec2::Normalize()
{
	float lengthSquared = GetLengthSquared();
	if (lengthSquared > 0.f)
	{
		*this = GetProjectedOnto2D(*this,*this) / GetLength();
	}
	else
	{
		*this = Vec2(1.f, 0.f);
	}
}

void Vec2::Reflect(Vec2 const& normal)
{
	float projectionLength = GetProjectedLength2D(*this, normal);
	Vec2  displacementInDirectionOfNormal = projectionLength * normal;
	*this = *this - 2.f * displacementInDirectionOfNormal;
}

Vec2 Vec2::GetReflected(Vec2 const& normal) const
{
	Vec2 reflected = *this;
	reflected.Reflect(normal);
	return reflected;
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();
	if (length > 0.f)
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
		return length;
	}
	return 0.f;
}
