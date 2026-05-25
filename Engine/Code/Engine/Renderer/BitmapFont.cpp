#include "BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
	: m_fontGlyphsSpriteSheet(SpriteSheet(fontTexture, IntVec2(16, 16)))
	, m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
{

}

Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& textString, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, TextBoxMode mode, int maxGlyphsToDraw)
{
	int returnCount		 = 0;
	int prevIndex		 = 0;
	float textBlockWidth = 0.f;

	for (int index = 0; index < textString.length(); ++index)
	{
		if (textString[index] == '\n')
		{
			float contendingWidth = GetTextWidth(cellHeight, textString.substr(prevIndex, index));
			if (contendingWidth > textBlockWidth)
			{
				textBlockWidth = contendingWidth;
			}
			++returnCount;
			prevIndex = index;
		}
	}

	if (textBlockWidth == 0.f)
	{
		textBlockWidth = GetTextWidth(cellHeight, textString);
	}

	float textBlockHeight = returnCount * cellHeight * (1.0f - alignment.y);

	if (mode == SHRINK)
	{
		if (textBlockWidth > (box.m_max.x - box.m_min.x) || textBlockHeight > (box.m_max.y - box.m_min.y))
		{
			float newCellHeight = MinFloat((box.m_max.x - box.m_min.x) / textBlockWidth, 
				(box.m_max.y - box.m_min.y) / textBlockHeight) * cellHeight;
			cellHeight = newCellHeight;
			textBlockHeight = returnCount * cellHeight * (1.0f - alignment.y);
		}
	}

	int lastIndex = static_cast<int>(textString.length());
	std::string currentLine;
	int drawnGlyphs = 0;
	for (int index = 0; index <= lastIndex; ++index)
	{
		if (index == lastIndex || textString[index] == '\n')
		{
			Vec2 anchor;

			anchor.x = RangeMap(alignment.x, 0.0f, 1.0f, box.m_min.x, box.m_max.x);
			anchor.y = RangeMap(alignment.y, 0.0f, 1.0f, box.m_min.y, box.m_max.y);

			float offsetY = (returnCount - 1 * alignment.y) * cellHeight - 2 * textBlockHeight;
			float offsetX = -textBlockWidth * alignment.x * 2.f;
			Vec2 origin;
			origin.x = offsetX;
			origin.y = -offsetY;

			Vec2 rawXY = anchor + origin;
			int drawnGlyphsContending = drawnGlyphs + static_cast<int>(currentLine.length());
			if (drawnGlyphsContending > drawnGlyphs)
			{
				std::string trailingText = currentLine.substr(0, size_t(maxGlyphsToDraw) - size_t(drawnGlyphs));

				float cellWidth = cellHeight * cellAspect;
				AABB2 cellBounds = AABB2(0, 0, cellWidth, cellHeight);
				AABB2 transformedCellBounds = cellBounds;
				for (int glyphIndex = 0; glyphIndex < trailingText.length(); ++glyphIndex)
				{
					char const& glyph = trailingText[glyphIndex];
					if (glyph != '\r')
					{
						int x = glyph % 16;
						int y = glyph / 16;
						SpriteDefinition def = m_fontGlyphsSpriteSheet.GetSpriteDefinition(x, 15 - y);
						AddVertsForAAB2DWithSprite(vertexArray, def, rawXY, tint, transformedCellBounds);
						transformedCellBounds.Translate(Vec2(cellWidth, 0.f));
					}
					else
					{
						transformedCellBounds.m_min.x = cellBounds.m_min.x;
						transformedCellBounds.m_max.x = cellBounds.m_max.x;
						transformedCellBounds.Translate(Vec2(0.f, -cellHeight));
					}
				}
			}
			else
			{
				float cellWidth = cellHeight * cellAspect;
				AABB2 cellBounds = AABB2(0, 0, cellWidth, cellHeight);
				AABB2 transformedCellBounds = cellBounds;
				for (int glyphIndex = 0; glyphIndex < currentLine.length(); ++glyphIndex)
				{
					char const& glyph = currentLine[glyphIndex];
					if (glyph != '\r')
					{
						int x = glyph % 16;
						int y = glyph / 16;
						SpriteDefinition def = m_fontGlyphsSpriteSheet.GetSpriteDefinition(x, 15 - y);
						AddVertsForAAB2DWithSprite(vertexArray, def, rawXY, tint, transformedCellBounds);
						transformedCellBounds.Translate(Vec2(cellWidth, 0.f));
					}
					else
					{
						transformedCellBounds.m_min.x = cellBounds.m_min.x;
						transformedCellBounds.m_max.x = cellBounds.m_max.x;
						transformedCellBounds.Translate(Vec2(0.f, -cellHeight));
					}
				}
			}
			drawnGlyphs = drawnGlyphsContending;

			currentLine.clear();
			++returnCount;
		}
		else
		{
			currentLine += textString[index];
		}
	}
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/)
{
	Vec2  origin = textMins;
	float cellWidth = cellHeight * cellAspect;
	AABB2 cellBounds = AABB2(0, 0, cellWidth, cellHeight);
	AABB2 transformedCellBounds = cellBounds;
	for (int index = 0; index < text.length(); ++index)
	{
		char const& glyph = text[index];
		if (glyph != '\r')
		{
			int x = glyph % 16;
			int y = glyph / 16;
			SpriteDefinition def = m_fontGlyphsSpriteSheet.GetSpriteDefinition(x, 15 - y);
			AddVertsForAAB2DWithSprite(vertexArray, def, origin, tint, transformedCellBounds);
			transformedCellBounds.Translate(Vec2(cellWidth, 0.f));
		}
		else
		{
			transformedCellBounds.m_min.x = cellBounds.m_min.x;
			transformedCellBounds.m_max.x = cellBounds.m_max.x;
			transformedCellBounds.Translate(Vec2(0.f, -cellHeight));
		}
	}
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect /*= 1.f*/)
{
	Strings lines = SplitStringOnDelimiter(text, '\n');
	float textWidth = 0.f;
	for (std::string line : lines)
	{
		line = RemoveSubstr(line, "\r");
		float cellWidth = cellHeight * cellAspect;
		float lineWidth = cellWidth * line.size();
		if (lineWidth > textWidth)
		{
			textWidth = lineWidth;
		}
	}

	return textWidth;
}

float BitmapFont::GetTextHeight(float cellHeight, std::string const& text, float cellAspect)
{
	UNUSED(cellAspect);
	Strings lines = SplitStringOnDelimiter(text, '\n');
	float textHeight = cellHeight * lines.size();

	return textHeight;
}

Vec2 BitmapFont::GetDenormalizedAnchorForBox(AABB2 const& box, Vec2 const& alignment)
{
	Vec2 anchor;
	anchor.x = RangeMap(alignment.x, 0.0f, 1.0f, box.m_min.x, box.m_max.x);
	anchor.y = RangeMap(alignment.y, 0.0f, 1.0f, box.m_min.y, box.m_max.y);
	return anchor;
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	return 1.f;
}

