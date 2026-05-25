#include "SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout, AABB2 const& bounds)
	: m_texture(&texture)
	, m_simpleGridLayout(simpleGridLayout)
	, m_textureRegion(bounds)
{
	m_cellWidth = 1.f / static_cast<float>(m_simpleGridLayout.x);
	m_cellHeight = 1.f / static_cast<float>(m_simpleGridLayout.y);
}

SpriteDefinition SpriteSheet::GetSpriteDefinition(int index) const
{
	int y = index / m_simpleGridLayout.x;
	int x = index - y * m_simpleGridLayout.x;
	return GetSpriteDefinition(x,y);
}

SpriteDefinition SpriteSheet::GetSpriteDefinition(IntVec2 const& coords) const
{
	return GetSpriteDefinition(coords.x, coords.y);
}

SpriteDefinition SpriteSheet::GetSpriteDefinition(int x, int y) const
{
	const float scale = 1.f / 128.f;
	Vec2 nudge = Vec2(m_cellWidth * scale, m_cellHeight * scale);
	Vec2 min = Vec2(m_cellWidth * x + nudge.x, m_cellHeight * y + nudge.y);
	Vec2 max = Vec2(m_cellWidth * (x + 1.f) - nudge.x, m_cellHeight * (y + 1.f) - nudge.y);

	Vec2 mappedMin = Vec2(RangeMap(min.x,0.f,1.f,m_textureRegion.m_min.x,m_textureRegion.m_max.x), RangeMap(min.y, 0.f, 1.f, m_textureRegion.m_min.y, m_textureRegion.m_max.y));
	Vec2 mappedMax = Vec2(RangeMap(max.x, 0.f, 1.f, m_textureRegion.m_min.x, m_textureRegion.m_max.x), RangeMap(max.y, 0.f, 1.f, m_textureRegion.m_min.y, m_textureRegion.m_max.y));
	return SpriteDefinition(
		m_texture, 
		AABB2(
			mappedMin, 
			mappedMax
		)
	);
}

Texture& SpriteSheet::GetTexture() const
{
	return *m_texture;
}
