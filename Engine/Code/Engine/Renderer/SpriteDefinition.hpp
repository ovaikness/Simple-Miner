#pragma once
#include "Texture.hpp"
#include "Engine/Math/AABB2.hpp"

class SpriteDefinition
{
public:
	SpriteDefinition();
	SpriteDefinition(Texture const* texture, AABB2 const& uvBounds);
	Texture const* GetTexture() const;
	AABB2		   GetUVBounds()   const;
protected:
	AABB2 m_uvBounds;
	Texture const* m_texture;
};