#include "Game/Voxels/VoxelCave.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/ThirdParty/Squirrel/RawNoise.hpp"
#include "Engine/ThirdParty/Squirrel/SmoothNoise.hpp"
Vec3 GetRandomDirection(RandomNumberGenerator& rng)
{
	float x = rng.RollRandomFloatInRange(-1.f, 1.f);
	float y = rng.RollRandomFloatInRange(-1.f, 1.f);
	float z = rng.RollRandomFloatInRange(-1.f, 1.f);

	return Vec3(x, y, z).GetNormalized();
}

VoxelCave::VoxelCave(IntVec3 const& startPos, float caveRadius, float maxRadius, uint32_t seed)
	: m_start(Vec3(startPos.x + 0.5f, startPos.y + 0.5f, startPos.z + 0.5f))
	, m_caveRadius(caveRadius)
	, m_radius(maxRadius)
	, m_startPos(startPos)
	, m_seed(seed)
{
}

void VoxelCave::Generate()
{
	float m_fuel = m_radius;
	RandomNumberGenerator rng(Get2dNoiseUint(m_startPos.x, m_startPos.y, m_seed));

	m_segs.push_back(m_start);
	Vec3 lastPos = m_start;
	float radiusSquared = 0.f;

	float pitch = 0;
	float yaw = rng.RollRandomFloatInRange(0.f, 360.f);
	float rYaw = yaw + 180.f;
	float backFuel = m_fuel;
	while (m_fuel > 0.f)
	{
		float distance = rng.RollRandomFloatInRange(7.f, 20.f);
		float deltaYaw = rng.RollRandomFloatInRange(-30.f, 30.f);
		float deltaPitch = rng.RollRandomFloatInRange(-15.f, 15.f);

		pitch += deltaPitch;
		yaw += deltaYaw;

		Vec3 newDir = Vec3::MakeFromPolarDegrees(pitch, yaw);
		Vec3 newPos = lastPos + newDir * distance;

		float distanceFromStartSquared = (newPos - m_start).GetLengthSquared();
		radiusSquared = MaxFloat(radiusSquared, distanceFromStartSquared);

		m_segs.push_back(newPos);

		lastPos = newPos;

		m_fuel -= distance;
	}

	m_backSegs.push_back(m_start);
	lastPos = m_start;
	yaw = rYaw;
	pitch = 0.f;

	while (backFuel < 0.f)
	{
		float distance = rng.RollRandomFloatInRange(7.f, 20.f);
		float deltaYaw = rng.RollRandomFloatInRange(-30.f, 30.f);
		float deltaPitch = rng.RollRandomFloatInRange(-15.f, 15.f);

		pitch += deltaPitch;
		yaw += deltaYaw;

		Vec3 newDir = Vec3::MakeFromPolarDegrees(pitch, yaw);
		Vec3 newPos = lastPos + newDir * distance;

		float distanceFromStartSquared = (newPos - m_start).GetLengthSquared();
		radiusSquared = MaxFloat(radiusSquared, distanceFromStartSquared);

		m_backSegs.push_back(newPos);

		lastPos = newPos;

		backFuel += distance;
	}
	//int subDivisions = 100;
	//float closestDistanceSquared = 999999999999999999999.f;
	//float parametricIncrement = 1.f / static_cast<float>(subDivisions);
	//Vec3 closestPoint = m_spline.EvaluateAtParametric(0.f);
	//for (size_t i = 0; i <= subDivisions; ++i)
	//{
	//	Vec3 currentPoint = m_spline.EvaluateAtParametric(static_cast<float>(i) * parametricIncrement);
	//	float currentDistanceSquared = GetDistanceSquared3D(point, currentPoint);
	//	if (currentDistanceSquared < closestDistanceSquared)
	//	{
	//		closestDistanceSquared = currentDistanceSquared;
	//		closestPoint = currentPoint;
	//	}
	//}
	//
	//return closestPoint;


	m_radius = sqrtf(radiusSquared) + m_caveRadius;
	m_valid = true;
}

bool VoxelCave::IsBlockInside(IntVec3 const& point, float radius) const
{
	Vec3 pointVec = Vec3(point.x + 0.5f, point.y + 0.5f, point.z + 0.5f);

	if (!DoSpheresOverlap(pointVec, 0.f, m_start, m_radius))
	{
		return false;
	}

	for (int i = 0; i < m_segs.size() - 1; i ++)
	{
		Vec3 const& start = m_segs.at(i);
		Vec3 const& end   = m_segs.at(i + 1);

		float radiusMag =  (0.5f * Compute3dPerlinNoise(point.x,point.y,point.z, 10.f, 3) * 0.5f) + 0.5f;

		if (GetDistanceSquared3D(pointVec, start) < m_caveRadius * m_caveRadius * radiusMag)
		{
			return true;
		}

		if (GetDistanceSquared3D(pointVec, end) < m_caveRadius * m_caveRadius * radiusMag)
		{
			return true;
		}

		Vec3 displacement = end - start;
		float distanceSquared = displacement.GetLengthSquared();

		Vec3 toPoint = pointVec - start;
		float dot = DotProduct3D(toPoint, displacement);
		Vec3 projection = GetProjectedOnto3D(toPoint, displacement);
		float projectionLengthSquared = projection.GetLengthSquared();

		if (dot < 0)
		{
			continue;
		}
		if (projectionLengthSquared > distanceSquared)
		{
			continue;
		}

		Vec3 rejection = toPoint - projection;
		float rejectionLengthSquared = rejection.GetLengthSquared();

		if (rejectionLengthSquared < m_caveRadius * m_caveRadius * radiusMag)
		{
			return true;
		}
	}
	return false;
}

bool VoxelCave::IsBlockInside(IntVec3 const& point) const
{
	return IsBlockInside(point, m_caveRadius);
}

bool VoxelCave::IsABB3Inside(AABB3 const& bounds) const
{
	return DoSphereAndAABBOverlap3D(m_start, m_radius, bounds);
}
