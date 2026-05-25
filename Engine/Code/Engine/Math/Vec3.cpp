#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/TinyXMLUtils.hpp"
#include <cmath>
//#include "Engine/Core/EngineCommon.hpp"

Vec3 Vec3::ZERO = Vec3(0.f, 0.f, 0.f);

//-----------------------------------------------------------------------------------------------
Vec3::Vec3(const Vec3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}

Vec3::Vec3(Vec2 const& copyFrom, float initialZ)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(initialZ)
{
}

Vec3::Vec3(Vec2 const&& copyFrom, float initialZ)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(initialZ)
{
}

Vec3::Vec3(float initialX, Vec2 const& copyFrom)
	:x (initialX)
	,y (copyFrom.x)
	,z (copyFrom.y)
{
}

Vec3::Vec3(float initialX, Vec2 const&& copyFrom)
	:x(initialX)
	, y(copyFrom.x)
	, z(copyFrom.y)
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3(float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + (const Vec3& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-(const Vec3& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(float uniformScale) const
{
	return Vec3(x * uniformScale, y * uniformScale, z * uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(const Vec3& vecToMultiply) const
{
	return Vec3(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/(float inverseScale) const
{
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=(const Vec3& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=(const Vec3& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=(const Vec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

float Vec3::GetLength() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vec3::GetLengthXY() const
{
	return sqrtf(GetLengthXYSquared());
}

float Vec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

float Vec3::GetLengthXYSquared() const
{
	return x * x + y * y;
}

float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y,x);
}

float Vec3::GetAngleAboutYDegrees() const
{
	if (x != 0 || z != 0)
	{
		return Atan2Degrees(x, z);
	}
	else
	{
		return 0.f;
	}
}

float Vec3::GetAngleAboutZRadians() const
{
	return atan2(y,x);
}

Vec3 Vec3::GetRotatedAboutZDegrees(float degrees) const
{
	if (x == 0 && y == 0) return *this;
	float length = GetLengthXY();
	float oldDegrees = Atan2Degrees(y, x);
	Vec3 rotated(length * CosDegrees(degrees + oldDegrees), length * SinDegrees(degrees + oldDegrees), z);
	return rotated;
}

Vec3 Vec3::GetRotatedAboutZRadians(float radians) const
{
	if (x == 0 && y == 0) return *this;
	float length = GetLengthXY();
	float oldRadians = atan2(y, x);
	Vec3 rotated(length * cosf(radians + oldRadians), length * sinf(radians + oldRadians), z);
	return rotated;
}

Vec3 Vec3::MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length)
{
	Vec3 result;
	result.z = length * CosDegrees(longitudeDegrees);
	result.y = length * SinDegrees(longitudeDegrees) * SinDegrees(latitudeDegrees);
	result.x = length * SinDegrees(longitudeDegrees) * CosDegrees(latitudeDegrees);
	return result;
}

Vec3 Vec3::GetNormalized() const
{
	Vec3 result = *this;
	result.Normalize();
	return result;
}

void Vec3::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	x = static_cast<float>(atof(strings.at(0).c_str()));
	y = static_cast<float>(atof(strings.at(1).c_str()));
	z = static_cast<float>(atof(strings.at(2).c_str()));
}

void Vec3::ClampLength(float maxLength)
{
	float oldLength = GetLength();
	if (oldLength > maxLength)
	{
		float scale = maxLength / oldLength;
		x *= scale;
		y *= scale;
		z *= scale;
	}
}

void Vec3::Normalize()
{
	float length = GetLength();
	float scale = 1.f / length;
	x *= scale;
	y *= scale;
	z *= scale;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*(float uniformScale, const Vec3& vecToScale)
{
	return Vec3(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==(const Vec3& compare) const
{
	return compare.x == x && compare.y == y && compare.z == z;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=(const Vec3& compare) const
{
	return compare.x != x || compare.y != y || compare.z != z;
}

