#include "Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

Vec4 const Vec4::ZERO = Vec4(0.f, 0.f, 0.f, 0.f);
Vec4 const Vec4::ONE = Vec4(1.f, 1.f, 1.f, 1.f);

Vec4::Vec4()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(0.0f)
{
}

Vec4::Vec4(Vec4 const& other) noexcept
	: x(other.x)
	, y(other.y)
	, z(other.z)
	, w(other.w)
{
}

Vec4::Vec4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

Vec4::Vec4(float scalar)
	: x(scalar)
	, y(scalar)
	, z(scalar)
	, w(scalar)
{

}

Vec4::Vec4(Vec2 const& other, float z /*= 0.f*/, float w /*= 1.f*/)
	: x(other.x)
	, y(other.y)
	, z(z)
	, w(w)
{

}

Vec4::Vec4(float x, Vec2 const& other, float w /*= 1.f*/)
	: x(x)
	, y(other.x)
	, z(other.y)
	, w(w)
{

}

Vec4::Vec4(float x, float y, Vec2 const& other)
	: x(x)
	, y(y)
	, z(other.x)
	, w(other.y)
{

}

Vec4::Vec4(Vec2 const& other1, Vec2 const& other2)
	: x(other1.x)
	, y(other1.y)
	, z(other2.x)
	, w(other2.y)
{

}

Vec4::Vec4(Vec3 const& other, float w /*= 1.f*/)
	: x(other.x)
	, y(other.y)
	, z(other.z)
	, w(w)
{

}

Vec4::Vec4(float x, Vec3 const& other)
	: x(x)
	, y(other.x)
	, z(other.y)
	, w(other.z)
{

}

Vec4 const Vec4::operator+(Vec4 const& other) const
{
	return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4 const Vec4::operator-(Vec4 const& other) const
{
	return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4 const Vec4::operator*(Vec4 const& other) const
{
	return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
}

Vec4 const Vec4::operator/(Vec4 const& other) const
{
	return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
}

Vec4 const Vec4::operator*(float scalar) const
{
	return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vec4 const Vec4::operator/(float scalar) const
{
	float scale = scalar;
	return Vec4(x / scale, y / scale, z / scale, w / scale);
}

std::string Vec4::ToString() const
{
	return std::string("Vec4(") + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
}

float Vec4::GetLength() const
{
	return sqrtf(GetLengthSquared());
}

float Vec4::GetLengthSquared() const
{
	return x * x + y * y + z * z + w * w;
}

void Vec4::Normalize()
{
	float length = GetLength();
	float scale = 1.f / length;
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vec4::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

Vec4 Vec4::GetNormalized() const
{
	float length = GetLength();
	float scale = 1.f / length;
	return Vec4(x * scale, y * scale, z * scale, w * scale);
}

Vec4 Vec4::GetNegated() const
{
	Vec4 result = *this;
	result.Negate();
	return result;
}

Vec4 Vec4::GetAbs() const
{
	Vec4 result = *this;
	result.x = fabsf(result.x);
	result.y = fabsf(result.y);
	result.z = fabsf(result.z);
	result.w = fabsf(result.w);
	return result;
}

Vec4 Vec4::GetMin(Vec4 const& other) const
{
	return Vec4(fminf(x, other.x), fminf(y, other.y), fminf(z, other.z), fminf(w, other.w));
}

Vec4 Vec4::GetMax(Vec4 const& other) const
{
	return Vec4(fmaxf(x, other.x), fmaxf(y, other.y), fmaxf(z, other.z), fmaxf(w, other.w));
}

Vec4 Vec4::GetLerped(Vec4 const& other, float t) const
{
	return Vec4(Interpolate(x, other.x, t), Interpolate(y, other.y, t), Interpolate(z, other.z, t), Interpolate(w, other.w, t));
}

Vec4 Vec4::GetReciprocal() const
{
	return Vec4(1.f / x, 1.f / y, 1.f / z, 1.f / w);
}

Vec4 Vec4::Lerp(Vec4 const& a, Vec4 const& b, float t)
{
	return Vec4(Interpolate(a.x, b.x, t), Interpolate(a.y, b.y, t), Interpolate(a.z, b.z, t), Interpolate(a.w, b.w, t));
}

Vec4 Vec4::Min(Vec4 const& a, Vec4 const& b)
{
	return Vec4(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w));
}

Vec4 Vec4::Max(Vec4 const& a, Vec4 const& b)
{
	return Vec4(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w));
}

bool Vec4::operator!=(Vec4 const& other) const
{
	return !(*this == other);
}

bool Vec4::operator==(Vec4 const& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

Vec4& Vec4::operator/=(float scalar)
{
	float scale = 1.f / scalar;
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;

	return *this;
}

Vec4& Vec4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

Vec4& Vec4::operator-=(Vec4 const& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

Vec4& Vec4::operator*=(Vec4 const& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;

	return *this;
}

Vec4& Vec4::operator/=(Vec4 const& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;

	return *this;
}

Vec4& Vec4::operator+=(Vec4 const& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

Vec4 const operator*(float scalar, Vec4 const& vec)
{
	return Vec4(scalar * vec.x, scalar * vec.y, scalar * vec.z, scalar * vec.w);
}
