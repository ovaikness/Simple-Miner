#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_Font.hpp"

#include <string>
#include <unordered_map>

class GlyphData
{
public:
	AABB2 m_planeBounds;
	AABB2 m_uvBounds;
	float m_spacing;
};

class MSDFFont
{
public:
	MSDFFont(Texture const* texture, std::string dataPath);

	Texture const* GetTexture() const;
	Vec2 GetTextDimensions(std::wstring const& text) const;

	void AddVertsForText(std::vector<Vertex_Font>& vertexArray, Vec2 const& textMins, float size, std::wstring const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, Vec4 const& effects = Vec4(0,0,0,0)) const;
	void AddVertsForTextInBox(std::vector<Vertex_Font>& vertexArray, AABB2 const& textBounds, Vec2 const& alignment,  float size, std::wstring const& text, Rgba8 const& tint /*= Rgba8::WHITE*/) const;
protected:
	float m_distanceRange = 2.f;
	float m_size = 32.f;
	float m_lineHeight = 1.f;

	Texture const* m_texture;
	std::unordered_map<wchar_t, GlyphData> m_glyphs;
};