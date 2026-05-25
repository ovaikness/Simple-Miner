#pragma once
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

struct Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;
	
	static Rgba8 WHITE;
	static Rgba8 BLACK;
	static Rgba8 RED;
	static Rgba8 GREEN;
	static Rgba8 BLUE;
public:
	explicit Rgba8();
	Rgba8(Vec4 normalizedColor)
	{
		r = DenormalizeByte(normalizedColor.x);
		g = DenormalizeByte(normalizedColor.y);
		b = DenormalizeByte(normalizedColor.z);
		a = DenormalizeByte(normalizedColor.w);
	};
	Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	void SetFromText(char const* text);

	Vec4 GetAsVec4() const;

	/*
	* @brief 
	* Expects a pointer and loads 4 floats forward from the pointer index.
	* (This is useful when loading color to a buffer when using a stride)
	*/
	void GetAsFloats(float* out_floats) const;
	bool operator==(Rgba8 const& compare) const
	{
		return r == compare.r && g == compare.g && b == compare.b && a == compare.a;
	};
	static Rgba8 Mix(Rgba8 begin, Rgba8 end, float t);
};