#include "Engine/Core/Vertex_Font.hpp"

Vertex_Font::Vertex_Font()
{

}

Vertex_Font::Vertex_Font(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec2 glyphPos, Vec2 texPos, int characterIndex, Vec4 effects)
	:m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_glyphPosition(glyphPos)
	, m_textPosition(texPos)
	, m_characterIndex(characterIndex)
	, m_effects(effects)
{
}