#include "IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "MathUtils.hpp"
#include <math.h>

IntVec2 const IntVec2::ZERO  = IntVec2(0, 0);
IntVec2 const IntVec2::UP    = IntVec2(0, 1);
IntVec2 const IntVec2::DOWN  = IntVec2(0, -1);
IntVec2 const IntVec2::LEFT  = IntVec2(-1, 0);
IntVec2 const IntVec2::RIGHT = IntVec2(1, 0);

IntVec2::IntVec2(const IntVec2& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{
}

IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{
}

bool IntVec2::operator==(const IntVec2& compare) const
{
	return x == compare.x && y == compare.y;
}

bool IntVec2::operator!=(const IntVec2& compare) const
{
	return x != compare.x || y != compare.y;
}

IntVec2 const IntVec2::operator+(const IntVec2& vecToAdd) const
{
	return IntVec2(x  + vecToAdd.x , y + vecToAdd.y);
}

IntVec2 const IntVec2::operator-(const IntVec2& vecToSubtract) const
{
	return IntVec2(x - vecToSubtract.x , y - vecToSubtract.y);
}

IntVec2 const IntVec2::operator-() const
{
	return IntVec2(-x,-y);
}

IntVec2 const IntVec2::operator*(int uniformScale) const
{
	return IntVec2(x * uniformScale, y * uniformScale);
}

IntVec2 const IntVec2::operator*(const IntVec2& vecToMultiply) const
{
	return IntVec2(x * vecToMultiply.x, y * vecToMultiply.y);
}

IntVec2 const IntVec2::operator/(int inverseScale) const
{
	return IntVec2(x / inverseScale, y / inverseScale);
}

void IntVec2::operator+=(const IntVec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void IntVec2::operator-=(const IntVec2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void IntVec2::operator*=(const int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}

void IntVec2::operator/=(const int uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

bool IntVec2::operator<(const IntVec2& compare) const
{
	if (x != compare.x)
	{
		return x < compare.x;
	}
	else if (y != compare.y)
	{
		return y < compare.y;
	}

	return false;
}

void IntVec2::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	x = atoi(strings.at(0).c_str());
	y = atoi(strings.at(1).c_str());
}

float IntVec2::GetLength() const
{
	return sqrtf(static_cast<float>(GetLengthSquared()));
}

int IntVec2::GetLengthSquared() const
{
	return x * x + y * y;
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees(static_cast<float>(y),static_cast<float>(x));
}

float IntVec2::GetOrientationRadians() const
{
	return atan2f(static_cast<float>(y),static_cast<float>(x));
}

IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y,x);
}

IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y,-x);
}

void IntVec2::Rotate90Degrees()
{
	int temp = x;
	x = -y;
	y = temp;
}

void IntVec2::RotateMinus90Degrees()
{
	int temp = x;
	x = y;
	y = -temp;
}

const IntVec2 operator*(int uniformScale, const IntVec2& vecToScale)
{
	return vecToScale * uniformScale;
}
