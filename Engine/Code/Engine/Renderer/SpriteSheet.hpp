#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Texture.hpp"
#include "SpriteDefinition.hpp"

class SpriteSheet
{
protected:
	Texture* m_texture = nullptr;
	AABB2    m_textureRegion = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	IntVec2  m_simpleGridLayout;

	float		   m_cellWidth;
	float		   m_cellHeight;
public:
	SpriteSheet() = default;
	SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout, AABB2 const& bounds = AABB2(0.f,0.f,1.f,1.f));
	SpriteDefinition GetSpriteDefinition(int index) const;
	SpriteDefinition GetSpriteDefinition(int x, int y) const;
	SpriteDefinition GetSpriteDefinition(IntVec2 const& coords) const;
	Texture&	     GetTexture() const;
};