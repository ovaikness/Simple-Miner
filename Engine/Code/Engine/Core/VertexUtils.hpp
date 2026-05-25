#pragma once
#include <vector>
#include <cmath>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/Vertex_Font.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/ConvexPoly2D.hpp"

#include <filesystem>

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};

void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& vertices, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& vertices, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translationXY);
void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);

void AddVertsForLine2D(std::vector<Vertex_PCU>& verts, Vec2 start, Vec2 end, Rgba8 const& color, float thickness);
void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness,Rgba8 color = Rgba8());
void AddVertsForDisc2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Vec2 const& center, float radius);
void AddVertsForRing2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Vec2 const& center, float radius, float thickness);
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Capsule2 const& capsule = Capsule2(Vec2(0, 0), Vec2(1, 0), 1.f));
void AddVertsForOBB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, Vec3 const& translation, float rotationDegrees, Rgba8 const& color, OBB2 const& bbox = OBB2(Vec2(0, 0), 1.f, 1.f, 0.f));
void AddVertsForOBB2D(std::vector<Vertex_PCU>& vertices, Vec3 const& translation, float rotationDegrees, Rgba8 const& color, OBB2 const& bbox = OBB2(Vec2(0,0),1.f,1.f,0.f));
void AddVertsForOBB2D(std::vector<Vertex_PCU>& vertices, OBB2 const& bbox, Rgba8 const& color = Rgba8::WHITE, float depth = 0.f);
void AddVertsForOBB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, OBB2 const& bbox, Rgba8 const& color = Rgba8::WHITE, float depth = 0.f, bool flipX = false, bool flipY = false);
void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& bbox = AABB2(Vec2(0,0),Vec2(1,1)));
void AddVertsForAABB2DOutline(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, AABB2 const& bbox = AABB2(Vec2(0, 0), Vec2(1, 1)), float thickness = 0.1f);
void AddVertsForAAB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& bbox = AABB2(Vec2(0, 0), Vec2(1, 1)));
void AddVertsForAABB2DWithGlyphSprite(std::vector<Vertex_Font>& vertices, SpriteDefinition const& spriteDef, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& textBox, int index, Vec4 const& effects, AABB2 const& box = AABB2(Vec2(0, 0), Vec2(1, 1)));
void AddVertsForPlane(std::vector<Vertex_PCU>& vertices, Plane3D const& plane);
void AddVertsForHex2D(std::vector<Vertex_PCU>& vertices, Vec2 const& center, float radius,float thickness, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForHexFilled2D(std::vector<Vertex_PCU>& vertices, Vec2 const& center, float radius, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForHex2D(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices, Vec2 const& center, float radius, float thickness, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForHexFilled2D(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices, Vec2 const& center, float radius, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForQuad3D(
	std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indices
	, Vec3 const& bottomLeft, Vec3 const& bottomRight
	, Vec3 const& topRight, Vec3 const& topLeft
	, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2(Vec2::ZERO, Vec2::ONE)
);

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts
	, Vec3 const& bottomLeft, Vec3 const& bottomRight
	, Vec3 const& topRight, Vec3 const& topLeft
	, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2(Vec2::ZERO,Vec2::ONE)
);

void AddVertsForAABB3D(
	std::vector<Vertex_PCU>& verts,
	AABB3 const& bounds,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(Vec2::ZERO, Vec2::ONE)
);

void AddVertsForOBB3D(
	std::vector<Vertex_PCU>& verts,
	OBB3 const& bounds,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(Vec2::ZERO, Vec2::ONE)
);

void AddVertsForSphere3D(
	std::vector<Vertex_PCU>& verts,
	Vec3 const& center,
	float radius,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(Vec2::ZERO, Vec2::ONE),
	int numSlices = 32,
	int numStacks = 16
);

void AddVertsForConvexPoly2D(
	std::vector<Vertex_PCU>& verts,
	ConvexPoly2D const& poly2D,
	Rgba8 const& color = Rgba8::WHITE
);

void AddVertsForHollowConvexPoly2D(
	std::vector<Vertex_PCU>& verts, 
	ConvexPoly2D const& poly2D, 
	Rgba8 const& color = Rgba8::WHITE,
	float thickness = 0.1f
);

void AddVertsForAABB3DIndexed(
	std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indices,
	AABB3 const& bounds,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(Vec2::ZERO, Vec2::ONE)
);

void AddVertsForSphere3DIndexed(
	std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indices,
	Vec3 const& center,
	float radius,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(Vec2::ZERO, Vec2::ONE),
	int numSlices = 64,
	int numStacks = 128
);

AABB2 GetVertexBounds2D(std::vector<Vertex_PCU> const& verts);

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts,
	Mat44 const& transform);

void AddVertsForCylinder3D(
	std::vector<Vertex_PCU>& verts,
	Vec3 const& start,
	Vec3 const& end,
	float radius,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& uvs = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)),
	int numSlices = 8
);

void AddVertsForCone3D(
	std::vector<Vertex_PCU>& verts,
	Vec3 const& start,
	Vec3 const& end,
	float radius,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& uvs = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)),
	int numSlices = 8
);

Mat44 GetBillboardMatrix(
	BillboardType billboardType,
	Mat44 const& targetMatrix,
	Vec3 const& billboardPosition,
	Vec2 const& billboardScale = Vec2(1.f, 1.f)
);

bool LoadObj(std::filesystem::path path, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasHasUvs, Mat44 const& transform = Mat44::IDENTITY);
bool LoadObjs(
	std::filesystem::path path, 
	std::vector<std::string>& out_materials, 
	std::vector<std::vector<Vertex_PCUTBN>>& out_verts, 
	std::vector<std::vector<unsigned int>>& out_indices, 
	std::vector<bool>& out_hasNormals, 
	std::vector<bool>& out_hasHasUvs, 
	Mat44 const& transform = Mat44::IDENTITY,
	int* numPositions = nullptr,
	int* numNormals = nullptr,
	int* numUVs = nullptr,
	int* numFaces = nullptr
);

bool LoadObjData(std::string_view data, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasHasUvs, Mat44 const& transform = Mat44::IDENTITY);
void CalculateTangentSpaceBasisVectors(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int> const& indices, bool computeNormals = true, bool computeTangents = true);