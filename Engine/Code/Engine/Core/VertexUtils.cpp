#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <optional>
#include <vector>

void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& vertices, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	TransformVertexArrayXY3D(static_cast<int>(vertices.size()), vertices.data(), uniformScaleXY, rotationDegreesAboutZ, translationXY);
}

void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	Vec3 translation(translationXY.x,translationXY.y,0.f);
	for (int i = 0; i < numVerts; ++i)
	{
		Vec3& position = verts[i].m_position;
		float length = sqrtf(position.x * position.x + position.y * position.y) * uniformScaleXY;
		float angleRadians = atan2f(position.y, position.x) + ConvertDegreesToRadians(rotationDegreesAboutZ);

		position = Vec3(length * cos(angleRadians), length * sin(angleRadians), 0.f);
		position += translation;
	}
}

void AddVertsForLine2D(std::vector<Vertex_PCU>& verts, Vec2 start, Vec2 end, Rgba8 const& color, float thickness)
{
	float halfThickness = thickness * 0.5f;
	Vec2 direction = (end - start).GetNormalized();
	Vec2 left = start - direction * halfThickness;
	Vec2 right = end + direction * halfThickness;
	Vec2 topLeft = left + direction.GetRotated90Degrees() * halfThickness;
	Vec2 topRight = right + direction.GetRotated90Degrees() * halfThickness;
	Vec2 bottomLeft = left + direction.GetRotatedMinus90Degrees() * halfThickness;
	Vec2 bottomRight = right + direction.GetRotatedMinus90Degrees() * halfThickness;


	verts.push_back(Vertex_PCU(Vec3(topLeft.x, topLeft.y, 0.f), color, Vec2()));
	verts.push_back(Vertex_PCU(Vec3(topRight.x, topRight.y, 0.f), color, Vec2()));
	verts.push_back(Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, 0.f), color, Vec2()));

	verts.push_back(Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, 0.f), color, Vec2()));
	verts.push_back(Vertex_PCU(Vec3(topRight.x, topRight.y, 0.f), color, Vec2()));
	verts.push_back(Vertex_PCU(Vec3(bottomRight.x, bottomRight.y, 0.f), color, Vec2()));

}

void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness,Rgba8 color)
{
	AddVertsForLine2D(verts, tailPos, tipPos, color, lineThickness);
	Vec2 displacement = tipPos - tailPos;
	Vec2 rightArrowTip = tipPos + Vec2::MakeFromPolarDegrees(displacement.GetOrientationDegrees() + 135.f, arrowSize);
	Vec2 leftArrowTip = tipPos + Vec2::MakeFromPolarDegrees(displacement.GetOrientationDegrees() - 135.f, arrowSize);
	AddVertsForLine2D(verts, tipPos, leftArrowTip, color, lineThickness);
	AddVertsForLine2D(verts, tipPos, rightArrowTip, color, lineThickness);
}

void AddVertsForDisc2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Vec2 const& center, float radius)
{
	int numSectors = 32;
	for (int i = 0; i < numSectors; ++i)
	{
		vertices.emplace_back(Vec3(center), color, Vec2());
		Vec2 second = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSectors)) * (i + 1), radius);
		vertices.emplace_back(Vec3(center + second), color, Vec2());
		Vec2 first = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSectors)) * i, radius);
		vertices.emplace_back(Vec3(center + first),color, Vec2());
	}
}

void AddVertsForRing2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Vec2 const& center, float radius, float thickness)
{
	int numSectors = 32;
	for (int i = 0; i < numSectors; ++i)
	{
		Vec2 first = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSectors)) * i, radius);
		Vec2 second = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSectors)) * (i + 1), radius);
		AddVertsForLine2D(vertices, first + center, second + center, color, thickness);
	}
}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, Capsule2 const& capsule)
{
	Vec2 start		  = capsule.m_bone.m_start;
	Vec2 end		  = capsule.m_bone.m_end;
	float radius	  = capsule.m_radius;
	Vec2 displacement = end - start;
	Vec2 iBasis = displacement.GetNormalized();
	Vec2 jBasis = iBasis.GetRotated90Degrees();

	//Box
	Vec2 topLeft     = jBasis * radius;
	Vec2 bottomLeft  = -topLeft;
	Vec2 topRight    = displacement + topLeft;
	Vec2 bottomRight = displacement + bottomLeft;
	
	vertices.push_back(Vertex_PCU(Vec3(bottomLeft + start), color, Vec2()));
	vertices.push_back(Vertex_PCU(Vec3(topRight + start), color, Vec2()));
	vertices.push_back(Vertex_PCU(Vec3(topLeft + start), color, Vec2()));
	
	vertices.push_back(Vertex_PCU(Vec3(bottomLeft + start), color, Vec2()));
	vertices.push_back(Vertex_PCU(Vec3(topRight + start), color, Vec2()));
	vertices.push_back(Vertex_PCU(Vec3(bottomRight + start), color, Vec2()));

	//Bottom Hemisphere
	int numSectors = 16;
	float degrees = jBasis.GetOrientationDegrees();
	for (int i = 0; i < numSectors; ++ i)
	{
		vertices.emplace_back(Vec3(start), color, Vec2());
		Vec2 first = Vec2::MakeFromPolarDegrees(((180.f / static_cast<float>(numSectors)) * i) + degrees, radius);
		vertices.emplace_back(Vec3(start + first), color, Vec2());
		Vec2 second = Vec2::MakeFromPolarDegrees(((180.f / static_cast<float>(numSectors)) * (i + 1)) + degrees, radius);
		vertices.emplace_back(Vec3(start + second), color, Vec2());
	}

	//Top Hemisphere
	degrees += 180.f;
	for (int i = 0; i < numSectors; ++ i)
	{
		vertices.emplace_back(Vec3(end), color, Vec2());
		Vec2 first = Vec2::MakeFromPolarDegrees(((180.f / static_cast<float>(numSectors)) * i) + degrees, radius);
		vertices.emplace_back(Vec3(end + first), color, Vec2());
		Vec2 second = Vec2::MakeFromPolarDegrees(((180.f / static_cast<float>(numSectors)) * (i + 1)) + degrees, radius);
		vertices.emplace_back(Vec3(end + second), color, Vec2());
	}
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& vertices, Vec3 const& translation, float rotationDegrees, Rgba8 const& color, OBB2 const& bbox)
{
	OBB2 rotatedBbox = bbox;
	rotatedBbox.Rotate(rotationDegrees);

	Vertex_PCU bl = Vertex_PCU(Vec3(rotatedBbox.GetBottomLeft())  + translation, color, Vec2(0.f, 0.f));
	Vertex_PCU br = Vertex_PCU(Vec3(rotatedBbox.GetBottomRight()) + translation, color, Vec2(1.f, 0.f));
	Vertex_PCU tl = Vertex_PCU(Vec3(rotatedBbox.GetTopLeft())	  + translation, color, Vec2(0.f, 1.f));
	Vertex_PCU tr = Vertex_PCU(Vec3(rotatedBbox.GetTopRight())    + translation, color, Vec2(1.f, 1.f));

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& vertices, OBB2 const& bbox, Rgba8 const& color, float depth)
{
	OBB2 bboxCopy = bbox;

	Vertex_PCU bl = Vertex_PCU(Vec3(bboxCopy.GetBottomLeft(), depth),  color, Vec2(0.f, 0.f));
	Vertex_PCU br = Vertex_PCU(Vec3(bboxCopy.GetBottomRight(), depth), color, Vec2(1.f, 0.f));
	Vertex_PCU tl = Vertex_PCU(Vec3(bboxCopy.GetTopLeft(), depth),     color, Vec2(0.f, 1.f));
	Vertex_PCU tr = Vertex_PCU(Vec3(bboxCopy.GetTopRight(), depth),    color, Vec2(1.f, 1.f));

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForOBB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, OBB2 const& bbox, Rgba8 const& color, float depth, bool flipX, bool flipY)
{
	AABB2 uvs = spriteDef.GetUVBounds();
	OBB2 bboxCopy = bbox;

	Vertex_PCU bl = Vertex_PCU(Vec3(bboxCopy.GetBottomLeft(), depth),	color, uvs.m_min);
	Vertex_PCU br = Vertex_PCU(Vec3(bboxCopy.GetBottomRight(), depth), color, Vec2(uvs.m_max.x, uvs.m_min.y));
	Vertex_PCU tl = Vertex_PCU(Vec3(bboxCopy.GetTopLeft(), depth),		color, Vec2(uvs.m_min.x, uvs.m_max.y));
	Vertex_PCU tr = Vertex_PCU(Vec3(bboxCopy.GetTopRight(), depth),	color, uvs.m_max);

	if (flipX)
	{
		std::swap(bl.m_uvTexCoords, br.m_uvTexCoords);
		std::swap(tl.m_uvTexCoords, tr.m_uvTexCoords);
	}

	if (flipY)
	{
		std::swap(bl.m_uvTexCoords, tl.m_uvTexCoords);
		std::swap(br.m_uvTexCoords, tr.m_uvTexCoords);
	}

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& bbox)
{
	Vertex_PCU bl = Vertex_PCU(Vec3(bbox.m_min.x + bottomLeft.x,bbox.m_min.y + bottomLeft.y,0),color,Vec2(0,0));
	Vertex_PCU br = Vertex_PCU(Vec3(bbox.m_max.x + bottomLeft.x,bbox.m_min.y + bottomLeft.y,0),color,Vec2(1,0));
	Vertex_PCU tl = Vertex_PCU(Vec3(bbox.m_min.x + bottomLeft.x,bbox.m_max.y + bottomLeft.y,0),color,Vec2(0,1));
	Vertex_PCU tr = Vertex_PCU(Vec3(bbox.m_max.x + bottomLeft.x,bbox.m_max.y + bottomLeft.y,0),color,Vec2(1,1));

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForAABB2DOutline(std::vector<Vertex_PCU>& vertices, Rgba8 const& color, AABB2 const& bbox, float thickness)
{
	AddVertsForLine2D(vertices, bbox.m_min, Vec2(bbox.m_min.x,bbox.m_max.y),  color, thickness);
	AddVertsForLine2D(vertices, bbox.m_min, Vec2(bbox.m_max.x, bbox.m_min.y), color, thickness);
	AddVertsForLine2D(vertices, Vec2(bbox.m_min.x, bbox.m_max.y), bbox.m_max, color, thickness);
	AddVertsForLine2D(vertices, Vec2(bbox.m_max.x, bbox.m_min.y), bbox.m_max, color, thickness);
}

void AddVertsForAAB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& bbox)
{
	AABB2 uvs = spriteDef.GetUVBounds();
	
	Vertex_PCU bl = Vertex_PCU(Vec3(bbox.m_min.x + bottomLeft.x, bbox.m_min.y + bottomLeft.y, 0), color, uvs.m_min);
	Vertex_PCU br = Vertex_PCU(Vec3(bbox.m_max.x + bottomLeft.x, bbox.m_min.y + bottomLeft.y, 0), color, Vec2(uvs.m_max.x,uvs.m_min.y));
	Vertex_PCU tl = Vertex_PCU(Vec3(bbox.m_min.x + bottomLeft.x, bbox.m_max.y + bottomLeft.y, 0), color, Vec2(uvs.m_min.x,uvs.m_max.y));
	Vertex_PCU tr = Vertex_PCU(Vec3(bbox.m_max.x + bottomLeft.x, bbox.m_max.y + bottomLeft.y, 0), color, uvs.m_max);

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForAABB2DWithGlyphSprite(std::vector<Vertex_Font>& vertices, SpriteDefinition const& spriteDef, Vec2 const& bottomLeft, Rgba8 const& color, AABB2 const& textBox, int index, Vec4 const& effects, AABB2 const& box)
{
	AABB2 uvs = spriteDef.GetUVBounds();

	Vertex_Font bl = Vertex_Font(Vec3(box.m_min.x + bottomLeft.x, box.m_min.y + bottomLeft.y, 0), color, uvs.m_min, Vec2(0,0),textBox.m_min,index,effects);
	Vertex_Font br = Vertex_Font(Vec3(box.m_max.x + bottomLeft.x, box.m_min.y + bottomLeft.y, 0), color, Vec2(uvs.m_max.x, uvs.m_min.y), Vec2(1.f,0.f), Vec2(textBox.m_max.x, textBox.m_min.y), index, effects);
	Vertex_Font tl = Vertex_Font(Vec3(box.m_min.x + bottomLeft.x, box.m_max.y + bottomLeft.y, 0), color, Vec2(uvs.m_min.x, uvs.m_max.y), Vec2(0.f,1.f), Vec2(textBox.m_min.x, textBox.m_max.y), index, effects);
	Vertex_Font tr = Vertex_Font(Vec3(box.m_max.x + bottomLeft.x, box.m_max.y + bottomLeft.y, 0), color, uvs.m_max, Vec2(1.f,1.f),textBox.m_max,index,effects);

	vertices.push_back(br);
	vertices.push_back(bl);
	vertices.push_back(tr);

	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(tl);
}

void AddVertsForPlane(std::vector<Vertex_PCU>& vertices, Plane3D const& plane)
{
	std::vector<Vertex_PCU> gridVerts;
	Mat44 transform = Mat44(plane.GetBitangentZUp(), plane.GetTangentZUp(), plane.m_normal, Vec3(0.f,0.f,0.f));

	AABB3 verticalLines = AABB3(Vec3(-0.005f, 0.f, -0.005f), Vec3(0.005f, 100.f, 0.005f));
	AABB3 horizontalLines = AABB3(Vec3(0.f, -0.005f, -0.005f), Vec3(100.f, 0.005f, 0.005f));

	AABB3 verticalLines5u = AABB3(Vec3(-0.01f, 0.f, -0.01f), Vec3(0.01f, 100.f, 0.01f));
	AABB3 horizontalLines5u = AABB3(Vec3(0.f, -0.01f, -0.01f), Vec3(100.f, 0.01f, 0.01f));


	AABB3 verticalLinesOrigin = AABB3(Vec3(-0.02f, 0.f, -0.02f), Vec3(0.02f, 100.f, 0.02f));
	AABB3 horizontalLinesOrigin = AABB3(Vec3(0.f, -0.02f, -0.02f), Vec3(100.f, 0.015f, 0.02f));

	for (int i = 0; i < 100; ++i)
	{
		Vec3 translation = Vec3((static_cast<float>(i) * 1.f) - 50.f, -50.f, 0.f);
		Vec3 hTranslation = Vec3(-50.f, (static_cast<float>(i) * 1.f) - 50.f, 0.f);
		AddVertsForAABB3D(gridVerts,
			verticalLines.GetTranslated(translation)
			, Rgba8(80, 80, 80)
		);
		AddVertsForAABB3D(gridVerts,
			horizontalLines.GetTranslated(hTranslation)
			, Rgba8(80, 80, 80)
		);
	}

	for (int i = 0; i < 20; ++i)
	{
		Vec3 translation = Vec3((static_cast<float>(i) * 5.f) - 50.f, -50.f, 0.f);
		Vec3 hTranslation = Vec3(-50.f, (static_cast<float>(i) * 5.f) - 50.f, 0.f);
		AddVertsForAABB3D(gridVerts,
			verticalLines5u.GetTranslated(translation)
			, Rgba8(0, 255, 0)
		);
		AddVertsForAABB3D(gridVerts,
			horizontalLines5u.GetTranslated(hTranslation)
			, Rgba8(255, 0, 0)
		);
	}

	AddVertsForAABB3D(gridVerts,
		verticalLinesOrigin.GetTranslated(Vec3(0.f, -50.f, 0.f))
		, Rgba8(0, 255, 0)
	);
	AddVertsForAABB3D(gridVerts,
		horizontalLinesOrigin.GetTranslated(Vec3(-50.f, 0.f, 0.f))
		, Rgba8(255, 0, 0)
	);

	for (Vertex_PCU& vertice : gridVerts)
	{
		vertice.m_position = transform.TransformPoint(vertice.m_position) + plane.m_distance * plane.m_normal;
		vertices.push_back(vertice);
	}
}

void AddVertsForHex2D(std::vector<Vertex_PCU>& vertices, Vec2 const& center, float radius,float thickness, Rgba8 const& color)
{
	float circumRadius = radius * 1.1547f;
	float outerRadius = circumRadius + thickness;
	float innerRadius = circumRadius - thickness;

	for (float theta = 0.f; theta < 360.f; theta += 60.f)
	{
		Vec2 outer = Vec2::MakeFromPolarDegrees(theta, outerRadius);
		Vec2 inner = Vec2::MakeFromPolarDegrees(theta, innerRadius);
		Vec2 nextOuter = Vec2::MakeFromPolarDegrees(theta + 60.f, outerRadius);
		Vec2 nextInner = Vec2::MakeFromPolarDegrees(theta + 60.f, innerRadius);

		vertices.push_back(Vertex_PCU(Vec3(center + outer), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + inner), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextInner), color, Vec2()));

		vertices.push_back(Vertex_PCU(Vec3(center + outer), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextInner), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextOuter), color, Vec2()));
	}
}

void AddVertsForHexFilled2D(std::vector<Vertex_PCU>& vertices, Vec2 const& center, float radius, Rgba8 const& color)
{
	float circumRadius = radius * 1.1547f;

	for (float theta = 0.f; theta < 360.f; theta += 60.f)
	{
		Vec2 outer = Vec2::MakeFromPolarDegrees(theta, circumRadius);
		Vec2 nextOuter = Vec2::MakeFromPolarDegrees(theta + 60.f, circumRadius);

		vertices.push_back(Vertex_PCU(Vec3(center), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextOuter), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + outer), color, Vec2()));
	}
}

void AddVertsForHex2D(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices, Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{
	float circumRadius = radius * 1.1547f;
	float outerRadius = circumRadius + thickness;
	float innerRadius = circumRadius - thickness;

	unsigned int startIndex = static_cast<unsigned int>(vertices.size());

	for (float theta = 0.f; theta < 360.f; theta += 60.f)
	{
		Vec2 outer = Vec2::MakeFromPolarDegrees(theta, outerRadius);
		Vec2 inner = Vec2::MakeFromPolarDegrees(theta, innerRadius);
		Vec2 nextOuter = Vec2::MakeFromPolarDegrees(theta + 60.f, outerRadius);
		Vec2 nextInner = Vec2::MakeFromPolarDegrees(theta + 60.f, innerRadius);

		vertices.push_back(Vertex_PCU(Vec3(center + outer), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + inner), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextOuter), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextInner), color, Vec2()));

		indices.push_back(startIndex + 0);
		indices.push_back(startIndex + 1);
		indices.push_back(startIndex + 3);

		indices.push_back(startIndex + 0);
		indices.push_back(startIndex + 3);
		indices.push_back(startIndex + 2);

		startIndex += 4;
	}
}

void AddVertsForHexFilled2D(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices, Vec2 const& center, float radius, Rgba8 const& color)
{
	float circumRadius = radius * 1.1547f;

	unsigned int startIndex = static_cast<unsigned int>(vertices.size());

	for (float theta = 0.f; theta < 360.f; theta += 60.f)
	{
		Vec2 outer = Vec2::MakeFromPolarDegrees(theta, circumRadius);
		Vec2 nextOuter = Vec2::MakeFromPolarDegrees(theta + 60.f, circumRadius);

		vertices.push_back(Vertex_PCU(Vec3(center), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + outer), color, Vec2()));
		vertices.push_back(Vertex_PCU(Vec3(center + nextOuter), color, Vec2()));

		indices.push_back(startIndex + 2);
		indices.push_back(startIndex + 1);
		indices.push_back(startIndex + 0);

		startIndex += 3;
	}
}

void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvs)
{
	AABB2 uvFlipped = AABB2(uvs.m_min.x, 1.f - uvs.m_max.y, uvs.m_max.x, 1.f - uvs.m_min.y);
	Vec3 normal = CrossProduct3D(topLeft - bottomLeft, bottomRight - bottomLeft).GetNormalized();
	Vec3 tangent = (bottomRight - bottomLeft).GetNormalized();
	Vec3 bitangent = (topLeft - bottomLeft).GetNormalized();
	Vertex_PCUTBN bl = Vertex_PCUTBN(bottomLeft, color, uvFlipped.m_min, tangent, bitangent, normal);
	Vertex_PCUTBN br = Vertex_PCUTBN(bottomRight, color, Vec2(uvFlipped.m_max.x, uvFlipped.m_min.y), tangent, bitangent, normal);
	Vertex_PCUTBN tl = Vertex_PCUTBN(topLeft, color, Vec2(uvFlipped.m_min.x, uvFlipped.m_max.y), tangent, bitangent, normal);
	Vertex_PCUTBN tr = Vertex_PCUTBN(topRight, color, uvFlipped.m_max, tangent, bitangent, normal);

	verts.push_back(bl);
	verts.push_back(br);
	verts.push_back(tr);
	verts.push_back(tl);

	int currentIndex;
	if (indices.size() > 0)
	{
		currentIndex = indices.back() + 1;
	}
	else
	{
		currentIndex = 0;
	}

	indices.push_back(currentIndex + 0);
	indices.push_back(currentIndex + 1);
	indices.push_back(currentIndex + 2);
	indices.push_back(currentIndex + 0);
	indices.push_back(currentIndex + 2);
	indices.push_back(currentIndex + 3);
}

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	//FORWARD
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z)
		, color
		, UVs
	);

	//BACK
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, color
		, UVs
	);

	//Top
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y,   bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x ,  bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x ,  bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y,   bounds.m_maxs.z)
		, color
		, UVs
	);

	//Bottom
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, color
		, UVs
	);

	//Front
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, color
		, UVs
	);


	//Back
	AddVertsForQuad3D(verts
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z)
		, color
	);
}

void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, OBB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	Mat44 transform = Mat44(bounds.GetIBasis(), bounds.GetJBasis(), bounds.GetKBasis(), Vec3(0.f, 0.f, 0.f));
	AABB3 aabb = AABB3(
		Vec3(-bounds.m_halfWidth,-bounds.m_halfLength,-bounds.m_halfHeight),
		Vec3(bounds.m_halfWidth,bounds.m_halfLength,bounds.m_halfHeight)
	);
	std::vector<Vertex_PCU> tempVerts;
	AddVertsForAABB3D(tempVerts, aabb, color, UVs);

	for (Vertex_PCU& vert : tempVerts)
	{
		vert.m_position = transform.TransformPoint(vert.m_position) + bounds.m_center;
		verts.push_back(vert);
	}
}

void AddVertsForSphere3D(
	std::vector<Vertex_PCU>& verts,
	Vec3 const& center,
	float radius,
	Rgba8 const& color,
	AABB2 const& UVs,
	int numSlices,
	int numStacks
)
{
	float const sliceAngle = 360.f / numSlices;
	float const stackAngle = 180.f / numStacks;

	float UvCellWidth =  (UVs.m_max.x - UVs.m_min.x) / numSlices;
	float UvCellHeight = (UVs.m_max.y - UVs.m_min.y) / numStacks;

	for (int xx = 0; xx < numSlices; ++xx)
	{
		for (int yy = 0; yy < numStacks; ++yy)
		{
			Vec3 bl = center + Vec3::MakeFromPolarDegrees(sliceAngle * xx, stackAngle * yy, radius);
			Vec3 br = center + Vec3::MakeFromPolarDegrees(sliceAngle * (xx + 1), stackAngle * yy, radius);
			Vec3 tl = center + Vec3::MakeFromPolarDegrees(sliceAngle * xx, stackAngle * (yy + 1), radius);
			Vec3 tr = center + Vec3::MakeFromPolarDegrees(sliceAngle * (xx + 1), stackAngle * (yy + 1), radius);

			AABB2 quadUvs = AABB2(
				UVs.m_min + Vec2(UvCellWidth * xx      , 1.f - UvCellHeight * yy),
				UVs.m_min + Vec2(UvCellWidth * (xx + 1), 1.f - UvCellHeight * (yy + 1))
			);

			AddVertsForQuad3D(
				verts,
				bl,
				br,
				tr,
				tl,
				color,
				quadUvs
			);
		}
	}
}

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, ConvexPoly2D const& poly2D, Rgba8 const& color)
{
	for (int i = 1; i < poly2D.GetPoints().size() - 1; ++i)
	{
		verts.push_back(Vertex_PCU(Vec3(poly2D.GetPoints()[0], 0.f), color, Vec2()));
		verts.push_back(Vertex_PCU(Vec3(poly2D.GetPoints()[i + 1], 0.f), color, Vec2()));
		verts.push_back(Vertex_PCU(Vec3(poly2D.GetPoints()[i], 0.f), color, Vec2()));
	}
}

void AddVertsForHollowConvexPoly2D(std::vector<Vertex_PCU>& verts, ConvexPoly2D const& poly2D, Rgba8 const& color, float thickness)
{
	for (int i = 1; i < poly2D.GetPoints().size(); ++i)
	{
		Vec2 start = poly2D.GetPoints()[i-1];
		Vec2 end = poly2D.GetPoints()[i];
		AddVertsForLine2D(verts, start, end, color, thickness);
	}

	AddVertsForLine2D(verts, poly2D.GetPoints().back(), poly2D.GetPoints().front(), color, thickness);
}

void AddVertsForAABB3DIndexed(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	//FORWARD
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, color
		, UVs
	);

	//BACK
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z)
		, color
		, UVs
	);

	//Top
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, color
		, UVs
	);

	//Bottom
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, color
		, UVs
	);

	//Front
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z)
		, color
		, UVs
	);


	//Back
	AddVertsForQuad3D(verts, indices
		, Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z)
		, Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z)
		, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z)
		, color
	);
}

void AddVertsForSphere3DIndexed(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices, int numStacks)
{
	float const sliceAngle = 360.f / numSlices;
	float const stackAngle = 180.f / numStacks;

	float UvCellWidth = (UVs.m_max.x - UVs.m_min.x) / numSlices;
	float UvCellHeight = (UVs.m_max.y - UVs.m_min.y) / numStacks;

	std::vector<Vertex_PCUTBN> prevRow;
	std::vector<Vertex_PCUTBN> currentRow;

	for (int yy = 0; yy <= numStacks; ++yy)
	{
		currentRow.clear();
		float phi = stackAngle * yy - 90.f; 
		float cosPhi = CosDegrees(phi);
		float sinPhi = SinDegrees(phi);

		for (int xx = 0; xx <= numSlices; ++xx)
		{
			float theta = sliceAngle * xx;
			float cosTheta = CosDegrees(theta);
			float sinTheta = SinDegrees(theta);

			Vertex_PCUTBN currentVert;
			Vec3 displacement = Vec3(cosTheta * cosPhi, sinTheta * cosPhi, sinPhi) * radius;
			Vec3 spherePos = displacement + center;

			currentVert.m_position = spherePos;
			currentVert.m_color = color;
			currentVert.m_uvTexCoords = Vec2(UVs.m_min.x + UvCellWidth * xx, UVs.m_min.y + UvCellHeight * yy);
			currentVert.m_normal = displacement.GetNormalized();
			currentVert.m_tangent = CrossProduct3D(Vec3(0.f, 0.f, 1.f), currentVert.m_normal).GetNormalized();
			currentVert.m_bitangent = CrossProduct3D(currentVert.m_normal, currentVert.m_tangent).GetNormalized();

			currentRow.push_back(currentVert);
		}

		if (!prevRow.empty())
		{
			for (int i = 0; i < numSlices; ++i)
			{
				Vertex_PCUTBN bl = prevRow[i];
				Vertex_PCUTBN br = prevRow[i + 1];
				Vertex_PCUTBN tl = currentRow[i];
				Vertex_PCUTBN tr = currentRow[i + 1];

				verts.push_back(bl);
				verts.push_back(br);
				verts.push_back(tr);
				verts.push_back(tl);

				int currentIndex = (int)verts.size() - 4;

				indices.push_back(currentIndex + 2);
				indices.push_back(currentIndex + 1);
				indices.push_back(currentIndex + 0);
				indices.push_back(currentIndex + 3);
				indices.push_back(currentIndex + 2);
				indices.push_back(currentIndex + 0);
			}
		}

		prevRow = currentRow;
	}
}


AABB2 GetVertexBounds2D(std::vector<Vertex_PCU> const& verts)
{
	if (verts.size() == 0)
	{
		ERROR_AND_DIE("Attempted to get bounds for empty vertex array!");
	}
	Vec2 min = Vec2(verts[0].m_position.x, verts[0].m_position.y);
	Vec2 max = Vec2(verts[0].m_position.x, verts[0].m_position.y);
	for (Vertex_PCU const& vertex : verts)
	{
		float x = vertex.m_position.x;
		float y = vertex.m_position.y;

		if (x < min.x)
		{
			min.x = x;
		}
		if (x > max.x)
		{
			max.x = x;
		}
		if (y < min.y)
		{
			min.y = y;
		}
		if (y > max.y)
		{
			max.y = y;
		}
	}
	return AABB2(min,max);
}

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform)
{
	for (Vertex_PCU& vertex : verts)
	{
		vertex.m_position = transform.TransformPosition3D(vertex.m_position);
	}
}

void AddVertsForCylinder3D(
	std::vector<Vertex_PCU>& verts, 
	Vec3 const& start, 
	Vec3 const& end, 
	float radius, 
	Rgba8 const& color, 
	AABB2 const& uvs, 
	int numSlices)
{
	std::vector<Vertex_PCU> cylinderVerts;
	Vec3 displacement = end - start;
	float height = displacement.GetLength();
	Vec2 start2D = Vec2(start.x, start.y);

	for (int i = 0; i < numSlices; ++i)
	{
		cylinderVerts.emplace_back(Vec3(0,0,0), color, Vec2());
		Vec2 first = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * i, radius);
		Vec2 second = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * (i + 1), radius);
		cylinderVerts.emplace_back(Vec3(first), color, Vec2());
		cylinderVerts.emplace_back(Vec3(second), color, Vec2());
	}

	float uvPerSlice = (uvs.m_max.x - uvs.m_min.x) / numSlices;
	for (int i = 0; i < numSlices; ++i)
	{
		Vec3 br = Vec3(Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * i - 90.f, radius), 0.f);
		Vec3 bl = Vec3(Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * (i + 1) - 90.f, radius), 0.f);
		Vec3 tl = Vec3(bl.x, bl.y, height);
		Vec3 tr = Vec3(br.x, br.y, height);

		AddVertsForQuad3D(cylinderVerts,
			bl,
			br,
			tr,
			tl,
			color,
			AABB2(Vec2(uvs.m_min.x + uvPerSlice * (i + 1), uvs.m_min.y) , Vec2(uvs.m_min.x + uvPerSlice * i, uvs.m_max.y))
		);
	}

	for (int i = 0; i < numSlices; ++i)
	{
		cylinderVerts.emplace_back(Vec3(0,0,height), color, Vec2());
		Vec2 first = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * i, radius);
		Vec2 second = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * (i + 1), radius);
		cylinderVerts.emplace_back(Vec3(second,height), color, Vec2());
		cylinderVerts.emplace_back(Vec3(first,height), color, Vec2());
	}
	
	Vec3 iBasis = displacement.GetNormalized();
	Vec3 jBasis;
	if (abs(1.f - abs(iBasis.z)) > 0.0001f)
	{
		jBasis = -CrossProduct3D(iBasis, Vec3(0.f, 0.f, 1.f));
	}
	else
	{
 		jBasis = -CrossProduct3D(iBasis, Vec3(1.f, 0.f, 0.f));
	}
	jBasis.Normalize();
	Vec3 kBasis = CrossProduct3D(jBasis, iBasis);
	Mat44 transform = Mat44(kBasis, jBasis, iBasis, start);
	TransformVertexArray3D(cylinderVerts, transform);

	for (Vertex_PCU& vertex : cylinderVerts)
	{
		verts.push_back(vertex);
	}
}

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvs, int numSlices)
{
	std::vector<Vertex_PCU> coneVerts;
	Vec3 displacement = end - start;
	float height = displacement.GetLength();
	Vec2 start2D = Vec2(start.x, start.y);

	for (int i = 0; i < numSlices; ++i)
	{
		coneVerts.emplace_back(Vec3(0, 0, 0), color, Vec2());
		Vec2 second = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * i, radius);
		Vec2 first = Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * (i + 1), radius);
		coneVerts.emplace_back(Vec3(second), color, Vec2());
		coneVerts.emplace_back(Vec3(first), color, Vec2());
	}

	float uvPerSlice = (numSlices / 360.f) * (uvs.m_max.x - uvs.m_min.x);
	for (int i = 0; i < numSlices; ++i)
	{
		Vec3 br = Vec3(Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * i, radius), 0.f);
		Vec3 bl = Vec3(Vec2::MakeFromPolarDegrees((360.f / static_cast<float>(numSlices)) * (i + 1), radius), 0.f);
		Vec3 tr = Vec3(0.f, 0.f, height);
		Vec3 tl = Vec3(0.f, 0.f, height);

		AddVertsForQuad3D(coneVerts,
			bl,
			br,
			tr,
			tl,
			color,
			AABB2(Vec2( uvs.m_min.x + uvPerSlice * i, 0.f), Vec2(  uvs.m_min.x + uvPerSlice * (i + 1), uvs.m_max.y))
		);
	}

	Vec3 iBasis = displacement.GetNormalized();
	Vec3 jBasis;
	if (abs(1.f - abs(iBasis.z)) > 0.0001f)
	{
		jBasis = -CrossProduct3D(iBasis, Vec3(0.f, 0.f, 1.f));
	}
	else
	{
		jBasis = -CrossProduct3D(iBasis, Vec3(0.f, 1.f, 0.f));
	}
	jBasis.Normalize();
	Vec3 kBasis = CrossProduct3D(jBasis, iBasis);
	Mat44 transform = Mat44(kBasis, jBasis, iBasis, start);
	TransformVertexArray3D(coneVerts, transform);

	for (Vertex_PCU& vertex : coneVerts)
	{
		verts.push_back(vertex);
	}
}

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& targetMatrix, Vec3 const& billboardPosition, Vec2 const& billboardScale)
{
	UNUSED(billboardScale);
	Mat44 result = Mat44::IDENTITY;
	result.SetTranslation3D(billboardPosition);
	switch (billboardType)
	{
	case BillboardType::FULL_FACING:
	{
		Vec3 targetTranslation = targetMatrix.GetTranslation3D();
		Vec3 displacement = targetTranslation - billboardPosition;
		Vec3 iBasis = displacement.GetNormalized();
		Vec3 kBasis = Vec3(0.f, 0.f, 1.f);
		float d = DotProduct3D(iBasis,kBasis);
		if (std::abs(d) == 1.f)
		{
			kBasis = Vec3(0.f, 1.f, 0.f);
		}
		kBasis -= d * iBasis;
		kBasis.Normalize();
		Vec3 jBasis = CrossProduct3D(kBasis, iBasis);

		result = Mat44(iBasis, jBasis, kBasis, billboardPosition);
		break;
	}
	case BillboardType::FULL_OPPOSING:
	{
		Vec3 iBasis = -targetMatrix.GetIBasis3D();
		Vec3 jBasis = -targetMatrix.GetJBasis3D();
		Vec3 kBasis = targetMatrix.GetKBasis3D();

		result = Mat44(iBasis, jBasis, kBasis, billboardPosition);
		break;
	}
	case BillboardType::WORLD_UP_FACING:
	{
		Vec3 targetTranslation = targetMatrix.GetTranslation3D();
		Vec3 kBasis = Vec3(0.f, 0.f, 1.f);
		Vec3 iBasis = targetTranslation - billboardPosition;
		iBasis.z = 0.f;
		iBasis.Normalize();
		Vec3 jBasis = Vec3(-iBasis.y, iBasis.x, 0.f);

		result = Mat44(iBasis, jBasis, kBasis, billboardPosition);
		break;
	}
	case BillboardType::WORLD_UP_OPPOSING:
	{
		Vec3 kBasis = Vec3(0.f, 0.f, 1.f);
		Vec3 iBasis = -targetMatrix.GetIBasis3D();
		iBasis.z = 0.f;
		iBasis.Normalize();
		Vec3 jBasis = Vec3(-iBasis.y, iBasis.x, 0.f);

		result = Mat44(iBasis, jBasis, kBasis, billboardPosition);
		break;
	}
	}
	return result;
}

bool LoadObj(std::filesystem::path path, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasHasUvs, Mat44 const& transform)
{
	out_hasNormals = false;
	out_hasHasUvs = false;

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;

	std::map<std::string, size_t> uniqueVerts;

	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indices;

	std::string fileString;

	int result = FileReadToString(fileString, path.string());
	if (result == -1)
	{
		return false;
	}

	//Remove scaling from normals
	Mat44 normalMatrix = transform.GetOrthonormalInverse().GetTranspose();

	std::vector<std::string> lines = SplitStringOnDelimiter(fileString, '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		//Position data
		if (tokens[0] == "v")
		{
			Vec3 vertex = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			positions.push_back(vertex);
		}
		else if (tokens[0] == "vt")
		{
			Vec2 uv = Vec2(std::stof(tokens[1]), std::stof(tokens[2]));
			uvs.push_back(uv);
		}
		else if (tokens[0] == "vn")
		{
			Vec3 normal = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			std::vector<unsigned int> faceIndices;
			for (size_t i = 1; i < tokens.size(); ++i)
			{


				std::vector<std::string> strIndices = SplitStringOnDelimiter(tokens[i], '/');
				size_t vertIndex = 0;
				if (!strIndices[0].empty())
				{
					vertIndex = std::stoul(strIndices[0]);
				}
				else
				{
					ERROR_RECOVERABLE("Face has no vertex index in file : " + fileString);
				}

				std::optional<size_t> uvIndex;
				if (strIndices.size() > 1)
					if (!strIndices[1].empty())
					{
						uvIndex = std::stoul(strIndices[1]);
					}

				std::optional<size_t> normalIndex;
				if (strIndices.size() > 2)
					if (!strIndices[2].empty())
					{
						normalIndex = std::stoul(strIndices[2]);
					}

				std::string vertKey = Stringf("%zu/%zu/%zu", vertIndex, uvIndex, normalIndex);
				if (uniqueVerts.find(vertKey) == uniqueVerts.end())
				{
					uniqueVerts[vertKey] = verts.size();
					Vertex_PCUTBN vert;
					vert.m_position = positions[vertIndex - 1];
					if (uvIndex.has_value())
					{
						vert.m_uvTexCoords = uvs[uvIndex.value() - 1];
						out_hasHasUvs = true;
					}
					else
					{
						vert.m_uvTexCoords = Vec2(0.0f, 0.0f);
					}

					if (normalIndex.has_value())
					{
						vert.m_normal = normals[normalIndex.value() - 1];
						out_hasNormals = true;
					}
					else
					{
						//#TODO : Calculate normals if they are not provided
						vert.m_normal = Vec3(0.0f, 0.0f, 0.0f);
					}

					verts.push_back(vert);
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
				else
				{
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
			}

			//Triangulate the face
			if (faceIndices.size() >= 3)
			{
				unsigned int rootIndex = faceIndices[0];
				for (size_t j = 2; j < faceIndices.size(); ++j)
				{
					indices.push_back(faceIndices[j - 1]);
					indices.push_back(rootIndex);
					indices.push_back(faceIndices[j]);
				}
			}
			else
			{
				ERROR_RECOVERABLE("Face has less than 3 vertices in file : " + fileString);
			}
		}
	}

	out_verts = verts;
	out_indices = indices;

	return true;
}

bool LoadObjs(
	std::filesystem::path path, 
	std::vector<std::string>& out_materials, 
	std::vector<std::vector<Vertex_PCUTBN>>& out_verts, 
	std::vector<std::vector<unsigned int>>& out_indices, 
	std::vector<bool>& out_hasNormals, 
	std::vector<bool>& out_hasHasUvs, 
	Mat44 const& transform,
	int* out_numPositions,
	int* out_numNormals,
	int* out_numUVs,
	int* out_numFaces
)
{
	if (out_numFaces)
	{
		*out_numFaces = 0;
	}

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;

	std::map<std::string, size_t> uniqueVerts;

	out_verts.clear();
	out_indices.clear();
	out_hasNormals.clear();

	out_materials.push_back("");
	out_verts.push_back(std::vector<Vertex_PCUTBN>());
	out_indices.push_back(std::vector<unsigned int>());
	out_hasNormals.push_back(false);
	out_hasHasUvs.push_back(false);

	int meshIndex = 0;

	std::string fileString = path.string();

	int result = FileReadToString(fileString, fileString);
	if (result == -1)
	{
		return false;
	}

	//Remove scaling from normals
	Mat44 normalMatrix = transform;

	std::vector<std::string> lines = SplitStringOnDelimiter(fileString, '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		for (auto itr = tokens.begin(); itr != tokens.end();)
		{
			auto token = *itr;
			if (token == "")
			{
				itr = tokens.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		if (tokens.size() == 0)
		{
			continue;
		}
		
		//Position data
		if (tokens[0] == "usemtl")
		{
			out_materials.push_back(tokens[1]);
			++meshIndex;
			out_verts.push_back(std::vector<Vertex_PCUTBN>());
			out_indices.push_back(std::vector<unsigned int>());
			out_hasNormals.push_back(false);
			out_hasHasUvs.push_back(false);
		}
		else if (tokens[0] == "v")
		{
			Vec3 vertex = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			positions.push_back(vertex);
		}
		else if (tokens[0] == "vt")
		{
			Vec2 uv = Vec2(std::stof(tokens[1]), std::stof(tokens[2]));
			uvs.push_back(uv);
		}
		else if (tokens[0] == "vn")
		{
			Vec3 normal = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			std::vector<unsigned int> faceIndices;
			for (size_t i = 1; i < tokens.size(); ++i)
			{


				std::vector<std::string> strIndices = SplitStringOnDelimiter(tokens[i], '/');
				size_t vertIndex = 0;
				if (!strIndices[0].empty())
				{
					vertIndex = std::stoul(strIndices[0]);
				}
				else
				{
					ERROR_RECOVERABLE("Face has no vertex index in file : " + fileString);
				}

				std::optional<size_t> uvIndex;
				if (strIndices.size() > 1)
					if (!strIndices[1].empty())
					{
						uvIndex = std::stoul(strIndices[1]);
					}

				std::optional<size_t> normalIndex;
				if (strIndices.size() > 2)
					if (!strIndices[2].empty())
					{
						normalIndex = std::stoul(strIndices[2]);
					}

				std::string vertKey = std::to_string(vertIndex) + "/" + std::to_string(uvIndex.value_or(0)) + "/" + std::to_string(normalIndex.value_or(0));
				if (uniqueVerts.find(vertKey) == uniqueVerts.end())
				{
					uniqueVerts[vertKey] = out_verts[meshIndex].size();
					Vertex_PCUTBN vert;
					vert.m_position = transform.TransformPosition3D(positions[vertIndex - 1]);
					if (uvIndex.has_value())
					{
						vert.m_uvTexCoords = uvs[uvIndex.value() - 1];
						out_hasHasUvs[meshIndex] = true;
					}
					else
					{
						vert.m_uvTexCoords = Vec2(0.0f, 0.0f);
					}

					if (normalIndex.has_value())
					{
						vert.m_normal = normalMatrix.TransformPosition3D(normals[normalIndex.value() - 1]);
						vert.m_normal.Normalize();
						out_hasNormals[meshIndex] = true;
					}
					else
					{
						vert.m_normal = Vec3(0.0f, 0.0f, 1.0f);
					}

					out_verts[meshIndex].push_back(vert);
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
				else
				{
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
			}

			//Triangulate the face
			if (faceIndices.size() >= 3)
			{
				unsigned int rootIndex = faceIndices[0];
				for (size_t j = 2; j < faceIndices.size(); ++j)
				{
					out_indices[meshIndex].push_back(faceIndices[j - 1]);
					out_indices[meshIndex].push_back(rootIndex);
					out_indices[meshIndex].push_back(faceIndices[j]);
				}
				if (out_numFaces != nullptr)
				{
					(*out_numFaces)++;
				}
			}
			else
			{
				ERROR_RECOVERABLE("Face has less than 3 vertices in file : " + fileString);
			}
		}
	}

	if (out_numPositions != nullptr)
	{
		*out_numPositions = (int)positions.size();
	}

	if (out_numNormals != nullptr)
	{
		*out_numNormals = (int)normals.size();
	}

	if (out_numUVs != nullptr)
	{
		*out_numUVs = (int)uvs.size();
	}

	return true;
}

bool LoadObjData(std::string_view data, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasHasUvs, Mat44 const& transform)
{
	out_hasNormals = false;
	out_hasHasUvs = false;

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;

	std::map<std::string, size_t> uniqueVerts;

	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indices;

	//Remove scaling from normals
	Mat44 normalMatrix = transform.GetOrthonormalInverse().GetTranspose();

	std::vector<std::string> lines = SplitStringOnDelimiter(std::string(data), '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		//Position data
		if (tokens[0] == "v")
		{
			Vec3 vertex = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			positions.push_back(vertex);
		}
		else if (tokens[0] == "vt")
		{
			Vec2 uv = Vec2(std::stof(tokens[1]), std::stof(tokens[2]));
			uvs.push_back(uv);
		}
		else if (tokens[0] == "vn")
		{
			Vec3 normal = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			std::vector<unsigned int> faceIndices;
			for (size_t i = 1; i < tokens.size(); ++i)
			{


				std::vector<std::string> strIndices = SplitStringOnDelimiter(tokens[i], '/');
				size_t vertIndex = 0;
				if (!strIndices[0].empty())
				{
					vertIndex = std::stoul(strIndices[0]);
				}
				else
				{
					ERROR_RECOVERABLE("Face has no vertex index in data!");
					return false;
				}

				std::optional<size_t> uvIndex;
				if (strIndices.size() > 1)
					if (!strIndices[1].empty())
					{
						uvIndex = std::stoul(strIndices[1]);
					}

				std::optional<size_t> normalIndex;
				if (strIndices.size() > 2)
					if (!strIndices[2].empty())
					{
						normalIndex = std::stoul(strIndices[2]);
					}

				std::string vertKey = Stringf("%zu/%zu/%zu", vertIndex, uvIndex, normalIndex);
				if (uniqueVerts.find(vertKey) == uniqueVerts.end())
				{
					uniqueVerts[vertKey] = verts.size();
					Vertex_PCUTBN vert;
					vert.m_position = positions[vertIndex - 1];
					if (uvIndex.has_value())
					{
						vert.m_uvTexCoords = uvs[uvIndex.value() - 1];
						out_hasHasUvs = true;
					}
					else
					{
						vert.m_uvTexCoords = Vec2(0.0f, 0.0f);
					}

					if (normalIndex.has_value())
					{
						vert.m_normal = normals[normalIndex.value() - 1];
						out_hasNormals = true;
					}
					else
					{
						//#TODO : Calculate normals if they are not provided
						vert.m_normal = Vec3(0.0f, 0.0f, 0.0f);
					}

					verts.push_back(vert);
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
				else
				{
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
			}

			//Triangulate the face
			if (faceIndices.size() >= 3)
			{
				unsigned int rootIndex = faceIndices[0];
				for (size_t j = 2; j < faceIndices.size(); ++j)
				{
					indices.push_back(faceIndices[j - 1]);
					indices.push_back(rootIndex);
					indices.push_back(faceIndices[j]);
				}
			}
			else
			{
				ERROR_RECOVERABLE("Face has less than 3 vertices in data!");
				return false;
			}
		}
	}

	out_verts = verts;
	out_indices = indices;

	return true;
}

Vec3 CalculateNormals(Vec3 const& a, Vec3 const& b, Vec3 const& c)
{
	Vec3 normal = CrossProduct3D(b - a, c - a);
	normal.Normalize();
	return normal;
}

void CalculateTangentSpaceBasisVectors(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int> const& indices, bool computeNormals, bool computeTangents)
{
	for (int i = 0; i < indices.size(); i += 3)
	{
		Vertex_PCUTBN& vert = verts[indices[i]];
		Vertex_PCUTBN& vert2 = verts[indices[i + 1]];
		Vertex_PCUTBN& vert3 = verts[indices[i + 2]];

		if (computeNormals)
		{
			Vec3 normal = CalculateNormals(vert.m_position, vert2.m_position, vert3.m_position);

			vert.m_normal = normal;
			vert2.m_normal = normal;
			vert3.m_normal = normal;
		}

		if (computeTangents)
		{
			Vec3 edge1 = vert2.m_position - vert.m_position;
			Vec3 edge2 = vert3.m_position - vert.m_position;

			Vec2 deltaUV1 = vert2.m_uvTexCoords - vert.m_uvTexCoords;
			Vec2 deltaUV2 = vert3.m_uvTexCoords - vert.m_uvTexCoords;

			float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			Vec3 tangent = f * Vec3(
				(deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
				(deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
				(deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
			);
			tangent -= DotProduct3D(tangent, vert.m_normal) * vert.m_normal;
			tangent.Normalize();

			Vec3 bitangent = f * Vec3(
				(deltaUV1.x * edge2.x - deltaUV2.x * edge1.x),
				(deltaUV1.x * edge2.y - deltaUV2.x * edge1.y),
				(deltaUV1.x * edge2.z - deltaUV2.x * edge1.z)
			);
			bitangent -= DotProduct3D(bitangent, vert.m_normal) * vert.m_normal;
			bitangent -= DotProduct3D(bitangent, tangent) * tangent;
			bitangent.Normalize();


			vert.m_tangent = tangent;
			vert2.m_tangent = tangent;
			vert3.m_tangent = tangent;

			vert.m_bitangent = bitangent;
			vert2.m_bitangent = bitangent;
			vert3.m_bitangent = bitangent;
		}
	}
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& vertices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvs)
{
	Vertex_PCU bl = Vertex_PCU(bottomLeft, color, uvs.m_min);
	Vertex_PCU br = Vertex_PCU(bottomRight, color, Vec2(uvs.m_max.x, uvs.m_min.y));
	Vertex_PCU tl = Vertex_PCU(topLeft, color, Vec2(uvs.m_min.x, uvs.m_max.y));
	Vertex_PCU tr = Vertex_PCU(topRight, color, uvs.m_max);

	vertices.push_back(bl);
	vertices.push_back(br);
	vertices.push_back(tr);

	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(tl);
}

void AddVertsForOBB2DWithSprite(std::vector<Vertex_PCU>& vertices, SpriteDefinition const& spriteDef, Vec3 const& translation, float rotationDegrees, Rgba8 const& color, OBB2 const& bbox)
{
	OBB2 rotatedBbox = bbox;
	rotatedBbox.Rotate(rotationDegrees);
	AABB2 uvs = spriteDef.GetUVBounds();

	Vertex_PCU bl = Vertex_PCU(Vec3(rotatedBbox.GetBottomLeft()) + translation, color, uvs.m_min);
	Vertex_PCU br = Vertex_PCU(Vec3(rotatedBbox.GetBottomRight()) + translation, color, Vec2(uvs.m_max.x, uvs.m_min.y));
	Vertex_PCU tl = Vertex_PCU(Vec3(rotatedBbox.GetTopLeft()) + translation, color, Vec2(uvs.m_min.x, uvs.m_max.y));
	Vertex_PCU tr = Vertex_PCU(Vec3(rotatedBbox.GetTopRight()) + translation, color, uvs.m_max);

	vertices.push_back(bl);
	vertices.push_back(br);
	vertices.push_back(tr);

	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(tl);
}

void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& vertices, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translationXY)
{
	for (auto itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		TransformPositionXY3D(itr->m_position, iBasis, jBasis, translationXY);
	}
}

