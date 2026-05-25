#include "Engine/Renderer/MSDFFont.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/TinyXMLUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

MSDFFont::MSDFFont(Texture const* texture, std::string dataPath)
	: m_texture(texture)
{
	Vec2 atlasSize;
	XmlDocument doc;
	if(doc.LoadFile(dataPath.c_str()) == tinyxml2::XML_SUCCESS)
	{
		XmlElement* root = doc.FirstChildElement("root");
		XmlElement* atlas = root->FirstChildElement("atlas");
		XmlElement* metrics = root->FirstChildElement("metrics");

		UNUSED(metrics);

		atlasSize.x = (float)atof(atlas->FirstChildElement("width")->GetText());
		atlasSize.y = (float)atof(atlas->FirstChildElement("height")->GetText());

		m_lineHeight	= ParseXMLAttribute(*atlas, "lineHeight", m_lineHeight);
		m_size			= ParseXMLAttribute(*atlas, "size",	m_size);
		m_distanceRange = ParseXMLAttribute(*atlas, "distanceRange", m_distanceRange);

		for (XmlElement* glyph = root->FirstChildElement("glyphs"); glyph; glyph = glyph->NextSiblingElement())
		{
			std::string name = glyph->Name();
			if (name != "glyphs")
			{
				continue;
			}

			wchar_t unicode = static_cast<wchar_t>(atoi(glyph->FirstChildElement("unicode")->GetText()));
			GlyphData data;

			XmlElement* uvBounds	  = glyph->FirstChildElement("atlasBounds");
			if (uvBounds)
			{
				data.m_uvBounds.m_min.x = (float)atof(uvBounds->FirstChildElement("left")->GetText()) / atlasSize.x;
				data.m_uvBounds.m_min.y = (float)atof(uvBounds->FirstChildElement("bottom")->GetText()) / atlasSize.y;
				data.m_uvBounds.m_max.x = (float)atof(uvBounds->FirstChildElement("right")->GetText()) / atlasSize.x;
				data.m_uvBounds.m_max.y = (float)atof(uvBounds->FirstChildElement("top")->GetText()) / atlasSize.y;
			}
			XmlElement* planeBounds = glyph->FirstChildElement("planeBounds");
			if (planeBounds)
			{
				data.m_planeBounds.m_min.x = (float)atof(planeBounds->FirstChildElement("left")->GetText());
				data.m_planeBounds.m_min.y = (float)atof(planeBounds->FirstChildElement("bottom")->GetText());
				data.m_planeBounds.m_max.x = (float)atof(planeBounds->FirstChildElement("right")->GetText());
				data.m_planeBounds.m_max.y = (float)atof(planeBounds->FirstChildElement("top")->GetText());
			}

			data.m_spacing	  = (float)atof(glyph->FirstChildElement("advance")->GetText());
			m_glyphs[unicode] = data;
		}
		
	}
}

Texture const* MSDFFont::GetTexture() const
{
	return m_texture;
}

Vec2 MSDFFont::GetTextDimensions(std::wstring const& text) const
{
	Vec2 dimensions = Vec2::ZERO;
	for (int index = 0; index < text.length(); ++index)
	{
		wchar_t glyph = text[index];
		if (glyph != L'\r')
		{
			if (m_glyphs.find(glyph) == m_glyphs.end())
			{
				glyph = L' ';
			}
			GlyphData const& glyphData = m_glyphs.at(glyph);
			dimensions.x += glyphData.m_spacing * m_size;
			float height = glyphData.m_planeBounds.GetDimensions().y * m_size;
			if (height > dimensions.y)
			{
				dimensions.y = height;
			}
		}
		else
		{
			dimensions.y -= m_size * m_lineHeight;
			dimensions.x = 0.f;
		}
	}
	return dimensions;
}

void MSDFFont::AddVertsForText(std::vector<Vertex_Font>& vertexArray, Vec2 const& textMins, float size, std::wstring const& text, Rgba8 const& tint, Vec4 const& effects) const
{
	Vec2 origin = textMins;
	Vec2 cursor = Vec2::ZERO;

	Vec2 dimensions = GetTextDimensions(text);

	for (int index = 0; index < text.length(); ++index)
	{
		wchar_t glyph = text[index];
		if (glyph != L'\r')
		{
			if (m_glyphs.find(glyph) == m_glyphs.end())
			{
				glyph = L' ';
			}

			GlyphData const& glyphData = m_glyphs.at(glyph);
			SpriteDefinition def(GetTexture(), glyphData.m_uvBounds);
			AABB2 glyphBounds = AABB2(glyphData.m_planeBounds.m_min * size, glyphData.m_planeBounds.m_max * size);
			glyphBounds.Translate(cursor);
			
			Vec2 min = glyphBounds.m_min;
			Vec2 max = glyphBounds.m_max;
			min.x /= dimensions.x;
			min.y /= dimensions.y;
			max.x /= dimensions.x;
			max.y /= dimensions.y;

			AddVertsForAABB2DWithGlyphSprite(vertexArray, def, origin, tint, AABB2(min,max), index,effects, glyphBounds);
			cursor.x += glyphData.m_spacing * size;
		}
		else
		{
			cursor.y -= size * m_lineHeight;
			cursor.x = 0.f;
		}
	}
}

void MSDFFont::AddVertsForTextInBox(std::vector<Vertex_Font>& vertexArray, AABB2 const& textBounds, Vec2 const& alignment, float size, std::wstring const& text, Rgba8 const& tint) const
{
	Vec2 cursor = Vec2::ZERO;
	float height = 0.f;
	for (int index = 0; index < text.length(); ++index)
	{
		wchar_t glyph = text[index];
		if (glyph != L'\r')
		{
			if (m_glyphs.find(glyph) == m_glyphs.end())
			{
				glyph = L' ';
			}

			GlyphData glyphData = m_glyphs.at(glyph);
			float contendingHeight = glyphData.m_planeBounds.GetDimensions().y * size;
			if (contendingHeight > height)
			{
				height = contendingHeight;
			}
			cursor.x += glyphData.m_spacing * size;
		}
		else
		{
			cursor.y -= size * m_lineHeight;
			cursor.x = 0.f;
		}
	}

	AABB2 printBounds = AABB2(0.f, -cursor.y, cursor.x, height);
	Vec2 offset;
	offset.x = (textBounds.GetDimensions().x - printBounds.GetDimensions().x) * alignment.x;
	offset.y = (textBounds.GetDimensions().y - printBounds.GetDimensions().y) * alignment.y;

	AddVertsForText(vertexArray, textBounds.m_min + offset, size, text, tint);
}
