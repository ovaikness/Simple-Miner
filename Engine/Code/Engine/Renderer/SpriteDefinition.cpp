#include "SpriteDefinition.hpp"
#include "Renderer.hpp"

SpriteDefinition::SpriteDefinition()
{
}

SpriteDefinition::SpriteDefinition(Texture const* texture, AABB2 const& uvBounds)
	: m_texture(texture)
	, m_uvBounds(uvBounds)
{
}

Texture const* SpriteDefinition::GetTexture() const
{
	return m_texture;
}

AABB2 SpriteDefinition::GetUVBounds() const
{
	return m_uvBounds;
}