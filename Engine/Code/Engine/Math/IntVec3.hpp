#pragma once

class IntVec3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

	IntVec3() = default;
	IntVec3(int initialX, int initialY, int initialZ) noexcept;

	bool operator==(const IntVec3& compare) const;
	bool operator!=(const IntVec3& compare) const;
	IntVec3 operator+(const IntVec3& rhs) const;
	IntVec3 operator-(const IntVec3& rhs) const;
	IntVec3 operator-() const;
	IntVec3 operator*(int rhs) const;
	IntVec3 operator/(int rhs) const;
	IntVec3& operator+=(const IntVec3& rhs);
	IntVec3& operator-=(const IntVec3& rhs);
	IntVec3& operator*=(int rhs);
	IntVec3& operator/=(int rhs);

	int GetLengthSquared() const;
};