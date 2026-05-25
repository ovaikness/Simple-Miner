#include "TiledHeatMap.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"

TiledHeatmap::~TiledHeatmap()
{
}

TiledHeatmap::TiledHeatmap(IntVec2 const& dimensions)
	:m_dimensions(dimensions)
{
	m_values.clear();
	m_values.reserve(static_cast<size_t>(m_dimensions.x) * static_cast<size_t>(m_dimensions.y));
	for (int y = 0; y < m_dimensions.y; ++y)
	{
		for (int x = 0; x < m_dimensions.x; ++x)
		{
			m_values.emplace_back(0.f);
		}
	}
}

void TiledHeatmap::SetAllValues(float value)
{
	for (int index = 0; index < m_values.size(); ++index)
	{
		m_values[index] = value;
	}
}

void TiledHeatmap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 bounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor) const
{
	Vec2 cellDimensions;
	cellDimensions.x = bounds.m_max.x / m_dimensions.x;
	cellDimensions.y = bounds.m_max.y / m_dimensions.y;
	for (size_t y = 0; y < size_t(m_dimensions.y); ++y)
	{
		for (size_t x = 0; x < size_t(m_dimensions.x); ++x)
		{
			float value = m_values[x + y * m_dimensions.x];

			Rgba8 color;
			if (value != specialValue)
			{
				float t = RangeMapClamped(value, valueRange.m_min, valueRange.m_max, 0.f, 1.f);
				color = Rgba8::Mix(lowColor, highColor, t);
			}
			else
			{
				color = specialColor;
			}

			AddVertsForAABB2D(verts, bounds.m_min + Vec2(static_cast<float>(x) * cellDimensions.x, static_cast<float>(y) * cellDimensions.y), color, AABB2(Vec2::ZERO,cellDimensions));
		}
	}
}
 
float TiledHeatmap::GetTileValue(int x, int y)
{
	int xSize = static_cast<int>(x);
	int ySize = static_cast<int>(y);
	int xMaxSize = static_cast<int>(m_dimensions.x) - 1;
	int yMaxSize = static_cast<int>(m_dimensions.y) - 1;
	if (xSize >= 0 && xSize <= xMaxSize
		&& ySize >= 0 && ySize <= yMaxSize)
	{
		return m_values[xSize + ySize * m_dimensions.x];
	}
	
	return 999999999999999.f;
}			

void TiledHeatmap::AddToTileValue(int x, int y, float value)
{
	int xSize = static_cast<int>(x);
	int ySize = static_cast<int>(y);
	int xMaxSize = static_cast<int>(m_dimensions.x) - 1;
	int yMaxSize = static_cast<int>(m_dimensions.y) - 1;
	if (xSize >= 0 && xSize <= xMaxSize
		&& ySize >= 0 && ySize <= yMaxSize)
	{
		m_values[xSize + ySize * m_dimensions.x] += value;
	}
}				  

void TiledHeatmap::SetTileValue(int x, int y, float value)
{
	int xSize = static_cast<int>(x);
	int ySize = static_cast<int>(y);
	int xMaxSize = static_cast<int>(m_dimensions.x) - 1;
	int yMaxSize = static_cast<int>(m_dimensions.y) - 1;
	if (xSize >= 0 && xSize <= xMaxSize
		&& ySize >= 0 && ySize <= yMaxSize)
	{
		m_values[xSize + ySize * m_dimensions.x] = value;
	}
}

