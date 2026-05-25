#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"

#include <vector>

class TiledHeatmap
{
public:
	~TiledHeatmap();
	TiledHeatmap(IntVec2 const& dimensions);

	void SetAllValues(float value);
	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 bounds, 
		FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor) const;
	float GetTileValue(int x, int y);
	void  AddToTileValue(int x, int y, float value);
	void  SetTileValue(int x, int y, float value);
protected:
	IntVec2			   m_dimensions;
	std::vector<float> m_values;
};