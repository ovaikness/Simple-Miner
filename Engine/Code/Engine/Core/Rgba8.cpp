#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

Rgba8 Rgba8::WHITE = Rgba8(255, 255, 255, 255);
Rgba8 Rgba8::BLACK = Rgba8(0, 0, 0, 255);
Rgba8 Rgba8::RED = Rgba8(255, 0, 0, 255);
Rgba8 Rgba8::GREEN = Rgba8(0, 255, 0, 255);
Rgba8 Rgba8::BLUE = Rgba8(0, 0, 255, 255);

Rgba8::Rgba8()
	: r(255)
	, g(255)
	, b(255)
	, a(255)
{
}

Rgba8::Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}

void Rgba8::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	r = static_cast<char>(atoi(strings.at(0).c_str()));
	g = static_cast<char>(atoi(strings.at(1).c_str()));
	b = static_cast<char>(atoi(strings.at(2).c_str()));
	if (strings.size() > 3)
	{
		a = static_cast<char>(atoi(strings.at(3).c_str()));
	}
}

Vec4 Rgba8::GetAsVec4() const
{
	Vec4 result;
	result.x = static_cast<float>(r) / 255.f;
	result.y = static_cast<float>(g) / 255.f;
	result.z = static_cast<float>(b) / 255.f;
	result.w = static_cast<float>(a) / 255.f;
	return result;
}

void Rgba8::GetAsFloats(float* out_floats) const
{
	out_floats[0] = static_cast<float>(r) / 255.f;
	out_floats[1] = static_cast<float>(g) / 255.f;
	out_floats[2] = static_cast<float>(b) / 255.f;
	out_floats[3] = static_cast<float>(a) / 255.f;
}

Rgba8 Rgba8::Mix(Rgba8 begin, Rgba8 end, float t)
{
	char r = static_cast<char>(Interpolate(float(begin.r), float(end.r), t));
	char g = static_cast<char>(Interpolate(float(begin.g), float(end.g), t));
	char b = static_cast<char>(Interpolate(float(begin.b), float(end.b), t));
	char a = static_cast<char>(Interpolate(float(begin.a), float(end.a), t));
	return Rgba8(r,g,b,a);
}
