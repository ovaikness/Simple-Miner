#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vertex_Font
{
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;
	Vec2 m_glyphPosition;
	Vec2 m_textPosition;
	int  m_characterIndex;

	Vec4 m_effects;

	Vertex_Font();
	explicit Vertex_Font(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec2 glyphPos, Vec2 texPos, int characterIndex, Vec4 effects);
};