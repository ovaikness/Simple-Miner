#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Image.hpp"

Image::~Image()
{

}

Image::Image(std::string const& relativeFilePath)
	:m_relativeFilePath(relativeFilePath)
{
	IntVec2 dimensions = IntVec2::ZERO;		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

									// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(true); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(relativeFilePath.c_str(), &dimensions.x, &dimensions.y, &bytesPerTexel, numComponentsRequested);

	m_dimensions = dimensions;

	// Check if the load was successful
	std::string errorMessage = "Failed to load texture " + std::string(relativeFilePath) + "!";
	ASSERT_RECOVERABLE(texelData, errorMessage);

	int size = dimensions.x * dimensions.y;
	m_texels = new Rgba8[size];
	for (int index = 0; index < size; ++index)
	{

		for (int byteIndex = 0; byteIndex < bytesPerTexel; ++byteIndex)
		{
			switch (byteIndex)
			{
				case 0:
				{
					//Red component
					m_texels[index].r = texelData[index * bytesPerTexel + byteIndex];
					break;
				}
				case 1:
				{
					//Green component
					m_texels[index].g = texelData[index * bytesPerTexel + byteIndex];
					break;
				}
				case 2:
				{
					//Blue Component
					m_texels[index].b = texelData[index * bytesPerTexel + byteIndex];
					break;
				}
				case 3:
				{
					//Alpa Component
					m_texels[index].a = texelData[index * bytesPerTexel + byteIndex];
					break;
				}
			}
		}
	}

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free(texelData);
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

std::string Image::GetRelativeFilepath() const
{
	return m_relativeFilePath;
}

Rgba8 Image::GetTexelAt(int x, int y) const
{
	GUARANTEE_OR_DIE(x >= 0 && x <= m_dimensions.x - 1
	&& y >= 0 && y <= m_dimensions.y - 1, 
		"Attempted to get texel outside of the dimensions of the image.");
	return m_texels[x + (m_dimensions.x * y)];
}

void Image::SetTexelAt(int x, int y, Rgba8 color)
{
	GUARANTEE_OR_DIE(x >= 0 && x <= m_dimensions.x - 1
	&& y >= 0 && y <= m_dimensions.y - 1,
		"Attempted to set texel outside of the dimensions of the image.");
	m_texels[x + (m_dimensions.x * y)] = color;
}

void* const Image::GetRawData() const
{
	return m_texels;
}
