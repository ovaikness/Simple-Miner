#include "Engine/Math/IntVec3.hpp"

IntVec3::IntVec3(int initialX, int initialY, int initialZ) noexcept
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{

}

IntVec3 IntVec3::operator+(const IntVec3& rhs) const
{
	IntVec3 result;
	result.x = x + rhs.x;
	result.y = y + rhs.y;
	result.z = z + rhs.z;

	return result;
}

IntVec3 IntVec3::operator-(const IntVec3& rhs) const
{
	IntVec3 result;
	result.x = x - rhs.x;
	result.y = y - rhs.y;
	result.z = z - rhs.z;

	return result;
}

IntVec3 IntVec3::operator-() const
{
	IntVec3 result;
	result.x = -x;
	result.y = -y;
	result.z = -z;

	return result;
}

IntVec3 IntVec3::operator*(int rhs) const
{
	IntVec3 result;
	result.x = x * rhs;
	result.y = y * rhs;
	result.z = z * rhs;

	return result;
}

IntVec3 IntVec3::operator/(int rhs) const
{
	IntVec3 result;
	result.x = x / rhs;
	result.y = y / rhs;
	result.z = z / rhs;

	return result;
}

IntVec3& IntVec3::operator/=(int rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;

	return *this;
}

int IntVec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

IntVec3& IntVec3::operator*=(int rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;

	return *this;
}

IntVec3& IntVec3::operator-=(const IntVec3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

IntVec3& IntVec3::operator+=(const IntVec3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

bool IntVec3::operator!=(const IntVec3& compare) const
{
	return x != compare.x || y != compare.y || z != compare.z;
}

bool IntVec3::operator==(const IntVec3& compare) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}
