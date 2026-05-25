#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class Image
{
public:
	~Image();
	Image() {};
	Image(std::string const& relativeFilePath);
	IntVec2		GetDimensions() const;
	std::string GetRelativeFilepath() const;
	Rgba8		GetTexelAt(int x, int y) const;
	void		SetTexelAt(int x, int y, Rgba8 color);

	void* const GetRawData() const;
protected:
	std::string m_relativeFilePath;
	IntVec2 m_dimensions;
	Rgba8*  m_texels = nullptr;
};