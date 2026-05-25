#pragma once
#include <vector>
#include <string.h>
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"

class Texture;

enum TextBoxMode
{
	SHRINK,
	OVERRUN
};

//------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK, int maxGlyphsToDraw = 99999999);

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);

	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f);
	float GetTextHeight(float cellHeight, std::string const& text, float cellAspect = 1.f);
	Vec2 GetDenormalizedAnchorForBox(AABB2 const& box, Vec2 const& alignment);
protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
};
