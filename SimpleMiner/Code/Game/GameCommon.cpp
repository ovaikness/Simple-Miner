#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <cmath>

void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	float halfThickness = thickness * 0.5f;
	Vec2 direction = (end - start).GetNormalized();
	Vec2 left = start - direction * halfThickness;
	Vec2 right = end + direction * halfThickness;
	Vec2 topLeft = left + direction.GetRotated90Degrees() * halfThickness;
	Vec2 topRight = right + direction.GetRotated90Degrees() * halfThickness;
	Vec2 bottomLeft = left + direction.GetRotatedMinus90Degrees() * halfThickness;
	Vec2 bottomRight = right + direction.GetRotatedMinus90Degrees() * halfThickness;
	Vertex_PCU vertices[6]
	{
		Vertex_PCU(Vec3(topLeft.x , topLeft.y , 0.f),color,Vec2()),
		Vertex_PCU(Vec3(topRight.x, topRight.y, 0.f),color,Vec2()),
		Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, 0.f),color,Vec2()),

		Vertex_PCU(Vec3(bottomLeft.x,bottomLeft.y,0.f),color,Vec2()),
		Vertex_PCU(Vec3(topRight.x, topRight.y,0.f), color,Vec2()),
		Vertex_PCU(Vec3(bottomRight.x, bottomRight.y,0.f),color,Vec2())
	};
	g_theRenderer->DrawVertexArray(6, vertices);
}

void DebugDrawRing(Vec2 position, float radius, float thickness, Rgba8 color)
{
	float distance = radius;
	for (int i = 0; i < 32; ++i)
	{
		float radians = (2 * M_PI / 32.f) * i;
		Vec2 start;
		start.x = cosf(radians) * distance;
		start.y = sinf(radians) * distance;
		start += position;

		Vec2 end;
		radians = (2 * M_PI / 32.f) * (i + 1);
		end.x = cosf(radians) * distance;
		end.y = sinf(radians) * distance;
		end += position;

		DebugDrawLine(start, end, thickness, color);
	}
}
