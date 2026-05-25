#include "MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/RaycastResult.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/Plane2D.hpp"
#include <cmath>

float GetDistance2D(Vec2 start, Vec2 end)
{
	return sqrtf(GetDistanceSquared2D(start,end));
}

float GetDistanceSquared2D(Vec2 start, Vec2 end)
{
	Vec2 displacement = Vec2(end.x - start.x, end.y - start.y);
	return displacement.x * displacement.x + displacement.y * displacement.y;
}

float GetDistanceXY3D(Vec3 start, Vec3 end)
{
	return GetDistance2D(Vec2(start.x,start.y), Vec2(end.x,end.y));
}

float GetDistanceXYSquared3D(Vec3 start, Vec3 end)
{
	return GetDistanceSquared2D(Vec2(start.x,start.y), Vec2(end.x,end.y));
}

float GetDistance3D(Vec3 start, Vec3 end)
{
	return sqrtf(GetDistanceSquared3D(start,end));
}

float GetDistanceSquared3D(Vec3 start, Vec3 end)
{
	Vec3 displacement = Vec3(end.x - start.x, end.y - start.y, end.z - start.z);
	return displacement.x * displacement.x + displacement.y * displacement.y + displacement.z * displacement.z;
}

float ConvertRadiansToDegrees(float radians)
{
	return radians  * 180.f / M_PI;
}

float ConvertDegreesToRadians(float degrees)
{
	return degrees * M_PI / 180.f;
}

float CosDegrees(float degrees)
{
	return cosf(ConvertDegreesToRadians(degrees));
}

float SinDegrees(float degrees)
{
	return sinf(ConvertDegreesToRadians(degrees));
}

float TanDegrees(float degrees)
{
	return tanf(ConvertDegreesToRadians(degrees));
}

float Atan2Degrees(float Y, float X)
{
	return ConvertRadiansToDegrees(atan2f(Y,X));
}

bool IsPointInDisc(Vec2 const& point, Vec2 const& center, float radius)
{
	return (point - center).GetLengthSquared() < radius * radius;
}

bool DoDiscsOverlap(Vec2 position, float radius, Vec2 otherPosition, float otherRadius)
{
	return GetDistanceSquared2D(position,otherPosition) < ( radius + otherRadius ) * ( radius + otherRadius );
}

bool DoSpheresOverlap(Vec3 position, float radius, Vec3 otherPosition, float otherRadius)
{
	return GetDistanceSquared3D(position,otherPosition) < ( radius + otherRadius ) * ( radius + otherRadius );
}

void TransformPosition2D(Vec2& position, float scale = 1, float rotation = 0, Vec2 const& translation = Vec2(0.f,0.f))
{
float length = position.GetLength() * scale;
float angleRadians = atan2f(position.y, position.x) + ConvertDegreesToRadians(rotation);

position = Vec2(length * cos(angleRadians), length * sin(angleRadians));
position += translation;
}

void TransformPositionXY3D(Vec3& position, float scale = 1, float rotation = 0, Vec2 const& translation = Vec2(0.f, 0.f))
{
	float length = position.GetLengthXY() * scale;
	float angleRadians = atan2f(position.y, position.x) + ConvertDegreesToRadians(rotation);

	position = Vec3(length * cos(angleRadians), length * sin(angleRadians), position.z);
	position += Vec3(translation.x, translation.y, 0.f);
}

void TransformPosition2D(Vec2& position, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	Vec2 iTranslate = iBasis * position.x;
	Vec2 jTranslate = jBasis * position.y;

	position		= translation + iTranslate + jTranslate;
}

void TransformPositionXY3D(Vec3& position, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation = Vec2(0.f,0.f))
{
	Vec2 xTranslate = iBasis * position.x;
	Vec2 yTranslate = jBasis * position.y;
	
	Vec2 positionXY = translation + xTranslate + yTranslate;

	position = Vec3(positionXY.x, positionXY.y, position.z);
}

float WrapDiscInRange(float position, float radius, float min, float max)
{
	float newPosition = 0.f;
	if (position > max - radius)
	{
		newPosition = min + radius + (position - (max + radius));
	}
	if (position < min + radius)
	{
		newPosition = max - radius + (position - (min + radius));
	}
	return newPosition;
}

float GetFractionWithinRange(float start, float end, float value)
{
	if (end - start == 0.f)
	{
		return 0.5f;
	}
	return (value - start) / (end - start);
}

float RangeMap(float value, float startOrigin, float endOrigin, float startDestination, float endDestination)
{
	float fraction = GetFractionWithinRange(startOrigin, endOrigin, value);
	return Interpolate(startDestination, endDestination, fraction);
}

float RangeMapClamped(float value, float startOrigin, float endOrigin, float startDestination, float endDestination)
{
	float fraction = GetFractionWithinRange(startOrigin, endOrigin, value);
	fraction = GetClampedZeroToOne(fraction);
	return Interpolate(startDestination, endDestination, fraction);
}

float MinFloat(float a, float b)
{
	if (a < b)
	{
		return a;
	}
	return b;
}

float MaxFloat(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	return b;
}

float GetClamped(float value, float min, float max)
{
	return MinFloat(max, MaxFloat(min, value));
}

float GetClampedZeroToOne(float value)
{
	return GetClamped(value, 0.f, 1.f);
}

int RoundDownToInt(float value)
{
	if (value > 0)
	{
		return static_cast<int>(value);
	}
	else if (value < 0)
	{
		return static_cast<int>(value) - 1;
	}
	return 0;
}

float GetShortestAngularDispDegrees(float angle, float targetAngle)
{
	targetAngle = fmodf(targetAngle, 360.f);
	angle = fmodf(angle, 360.f);
	float angleDisplacementForward = targetAngle - angle;
	float angleDisplacementBackward = targetAngle - 360.f - angle;
	float angleDisplacementBackward2 = targetAngle + 360.f - angle;
	if (abs(angleDisplacementBackward) > abs(angleDisplacementBackward2))
	{
		angleDisplacementBackward = angleDisplacementBackward2;
	}
	if (abs(angleDisplacementForward) < abs(angleDisplacementBackward))
	{
		return angleDisplacementForward;
	}
	return angleDisplacementBackward;
}

float SignFloat(float value)
{
	if (value > 0.f)
	{
		return 1;
	}
	if (value < 0.f)
	{
		return -1;
	}
	return 0;
}

float GetTurnedTowardDegrees(float angle, float targetAngle, float amount)
{
	targetAngle = fmodf(targetAngle, 360.f);
	angle = fmodf(angle, 360.f);
	float displacementDegrees = GetShortestAngularDispDegrees(angle, targetAngle);
	displacementDegrees = GetClamped(displacementDegrees, -amount, amount);
	
	return angle + displacementDegrees;
}

float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.x + a.y * b.y;
}

float GetProjectedLength2D(Vec2 const& a, Vec2 const& b)
{
	if (b.x == 0 && b.y == 0)
	{
		return 0.f;
	}
	Vec2 normalizedB = b.GetNormalized();
	return DotProduct2D(a,normalizedB);
}

Vec2 GetProjectedOnto2D(Vec2 const& a, Vec2 const& b) 
{
	Vec2 projection = DotProduct2D(b, a) * b / b.GetLengthSquared();
	return projection;
}

Vec3 GetProjectedOnto3D(Vec3 const& a, Vec3 const& b)
{
	Vec3 projection = DotProduct3D(b, a) * b / b.GetLengthSquared();
	return projection;
}

float GetRejectedLength2D(Vec2 const& a, Vec2 const& b)
{
	if (b.x == 0 && b.y == 0)
	{
		return a.GetLength();
	}
	Vec2 normalizedB = b.GetNormalized();
	return CrossProduct2D(a,normalizedB);
}

Vec2 GetRejectedOnto2D(Vec2 const& a, Vec2 const& b)
{
	Vec2 rejection = a - a * DotProduct2D(a,b);
	return rejection;
}

Vec3 GetRejectedOnto3D(Vec3 const& a, Vec3 const& b)
{
	Vec3 rejection = CrossProduct3D(a, b) * a / a.GetLengthSquared();
	return Vec3();
}

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
	return ConvertRadiansToDegrees(atan2f(CrossProduct2D(a, b), DotProduct2D(a, b)));
}

int GetTaxicabDistance2D(IntVec2 const& a, IntVec2 const& b)
{
	return (b - a).GetTaxicabLength();
}

bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float degreesOrientation, float degreesAperture, float radius)
{
	if (GetDistanceSquared2D(point, sectorTip) > radius * radius)
	{
		return false;
	}
	return IsPointInsideDirectedSector2D(point,sectorTip,Vec2::MakeFromPolarDegrees(degreesOrientation),degreesAperture,radius);
}

bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& direction, float degreesAperture, float radius)
{
	if (GetDistanceSquared2D(point, sectorTip) > radius * radius)
	{
		return false;
	}

	Vec2 pointVector = point - sectorTip;
	float angle = GetAngleDegreesBetweenVectors2D(pointVector, direction);
	if (angle < -degreesAperture / 2.f || angle > degreesAperture / 2.f)
	{
		return false;
	}

	return true;
}

bool IsPointInsideHex2D(Vec2 const& point, Vec2 const& center, float radius)
{
	// Calculate displacement of the point relative to the hexagon's center
	Vec2 displacement = point - center;

	for (float theta = 0.f; theta < 360.f; theta += 60.f)
	{
		Vec2 hexPoint = center + Vec2(radius * CosDegrees(theta) * 1.1547f, radius * SinDegrees(theta) * 1.1547f);
		Vec2 nextHexPoint = center + Vec2(radius * CosDegrees(theta + 60.f) * 1.1547f, radius * SinDegrees(theta + 60.f) * 1.1547f);

		Vec2 edge = nextHexPoint - hexPoint;
		Vec2 toPoint = point - hexPoint;

		float cross = CrossProduct2D(edge, toPoint);
		if (cross < 0)
		{
			return false;
		}
	}

	return true;
}

Vec2 GetNearestPointOnInfiniteLine(Vec2 const& point, LineSegment2 const& lineSeg)
{
	Vec2 centeredPoint = point - lineSeg.m_start;
	Vec2 segDisplacement = lineSeg.m_end - lineSeg.m_start;

	return lineSeg.m_start + GetProjectedOnto2D(centeredPoint, segDisplacement);
}

Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, LineSegment2 const& lineSeg)
{
	Vec2 centeredPoint = point - lineSeg.m_start;
	Vec2 segDisplacement = lineSeg.m_end - lineSeg.m_start;

	float centeredPointDifferenceFromStart = DotProduct2D(centeredPoint, segDisplacement);

	if (centeredPointDifferenceFromStart <= 0.f)
	{
		return lineSeg.m_start;
	}

	Vec2 endCenteredPoint = point - lineSeg.m_end;
	float endCenteredPointDifferenceFromEnd = DotProduct2D(endCenteredPoint, segDisplacement);

	if (endCenteredPointDifferenceFromEnd >= 0.f)
	{
		return lineSeg.m_end;
	}

	return lineSeg.m_start + GetProjectedOnto2D(centeredPoint, segDisplacement);
}

Vec3 GetNearestPointOnLineSegment3D(Vec3 const& point, Vec3 const& start, Vec3 const& end)
{
	Vec3 centeredPoint = point - start;
	Vec3 segDisplacement = end - start;

	float centeredPointDifferenceFromStart = DotProduct3D(centeredPoint, segDisplacement);

	if (centeredPointDifferenceFromStart <= 0.f)
	{
		return start;
	}

	Vec3 endCenteredPoint = point - end;

	float endCenteredPointDifferenceFromEnd = DotProduct3D(endCenteredPoint, segDisplacement);

	if (endCenteredPointDifferenceFromEnd >= 0.f)
	{
		return end;
	}

	return start + GetProjectedOnto3D(centeredPoint, segDisplacement);
}

Vec3 GetNearestPointOnAABB3(Vec3 const& point, AABB3 const& aabb)
{
	Vec3 newPoint = point;

	if (point.x < aabb.m_mins.x)
	{
		newPoint.x = aabb.m_mins.x;
	}
	if (point.x > aabb.m_maxs.x)
	{
		newPoint.x = aabb.m_maxs.x;
	}
	if (point.y < aabb.m_mins.y)
	{
		newPoint.y = aabb.m_mins.y;
	}
	if (point.y > aabb.m_maxs.y)
	{
		newPoint.y = aabb.m_maxs.y;
	}
	if (point.z < aabb.m_mins.z)
	{
		newPoint.z = aabb.m_mins.z;
	}
	if (point.z > aabb.m_maxs.z)
	{
		newPoint.z = aabb.m_maxs.z;
	}

	return newPoint;
}

Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& obb)
{
	return obb.GetNearestPoint(point);
}

Vec3 GetNearestPointOnZCylinder(Vec3 const& point, Vec2 const& base, FloatRange zHeight, float radius)
{
	Vec3 newPoint = point;

	if (point.z > zHeight.m_max)
	{
		newPoint.z = zHeight.m_max;
	}

	if (point.z < zHeight.m_min)
	{
		newPoint.z = zHeight.m_min;
	}

	Vec2 pointBase = Vec2(point.x, point.y);
	Vec2 discNearest = GetNearestPointOnDisc2D(pointBase, base, radius);

	newPoint.x = discNearest.x;
	newPoint.y = discNearest.y;

	return newPoint;
}

Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Capsule2 const& capsule)
{
	LineSegment2 bone = capsule.m_bone;
	float		 radius = capsule.m_radius;

	Vec2         nearest = GetNearestPointOnLineSegment2D(point, bone);

	if (IsPointInDisc(point, nearest, radius))
	{
		return point;
	}
	else
	{
		return GetNearestPointOnDisc2D(point, nearest, radius);
	}
}

bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule)
{
	LineSegment2 bone    = capsule.m_bone;
	float		 radius  = capsule.m_radius;

	Vec2         nearest = GetNearestPointOnLineSegment2D(point, bone);

	return IsPointInDisc(point, nearest, radius);
}

bool IsPointInsideOBB2(Vec2 const& point, OBB2 const& obb)
{
	return obb.IsPointInside(point);
}

Vec3 GetNearestPointOnSphere3D(Vec3 const& point, Vec3 const& center, float radius)
{
	Vec3 displacement = point - center;
	displacement.ClampLength(radius);
	return center + displacement;
}

Vec2 GetNearestPointOnDisc2D(Vec2 const& point, Vec2 const& center, float radius)
{
	Vec2 displacement = point - center;
	displacement.ClampLength(radius);
	return center + displacement;
}

bool PushDiscOutOfCapsule2D(Vec2& center, float radius, Capsule2 const& capsule, Vec2* out_minimumDisplacement)
{
	Vec2 point = GetNearestPointOnCapsule2D(center, capsule);
	return PushDiscOutOfPoint2D(center,radius,point,out_minimumDisplacement);
}

bool PushDiscOutOfOBB2D(Vec2& center, float radius, OBB2 const& obb2, Vec2* out_minimumDisplacement)
{
	Vec2 point = GetNearestPointOnOBB2D(center, obb2);
	return PushDiscOutOfPoint2D(center,radius,point,out_minimumDisplacement);
}

bool PushDiscOutOfPoint2D(Vec2& center, float radius, Vec2 const& point, Vec2* out_minimumDisplacement/* = nullptr*/)
{
	return PushDiscOutOfDisc2D(center,radius,point,0.f, out_minimumDisplacement);
}

bool PushDiscOutOfDisc2D(Vec2& center, float radius, Vec2 const& otherCenter, float otherRadius, Vec2* out_minimumDisplacement /*= nullptr*/)
{
	if (!DoDiscsOverlap(center, radius, otherCenter, otherRadius))
	{
		if (out_minimumDisplacement)
		{
			*out_minimumDisplacement = Vec2(0.f,0.f);
		}
		return false;
	}

	Vec2  displacement = center - otherCenter;
	float minimumTranslationValue = sqrtf((radius + otherRadius) * (radius + otherRadius)) - displacement.GetLength();
	Vec2 minimumDisplacement = displacement.GetNormalized() * minimumTranslationValue;
	if (out_minimumDisplacement)
	{
		*out_minimumDisplacement = minimumDisplacement;
	}
	center += minimumDisplacement;
	return true;
}

bool PushDiscsOutOfEachOther2D(Vec2& center, float radius, Vec2& otherCenter, float otherRadius, Vec2* out_minimumDisplacement/* = nullptr*/)
{
	if (!DoDiscsOverlap(center, radius, otherCenter, otherRadius))
	{
		if (out_minimumDisplacement)
		{
			*out_minimumDisplacement = Vec2(0.f, 0.f);
		}
		return false;
	}

	Vec2 displacement = center - otherCenter;
	float minimumTranslationValue = sqrtf((radius + otherRadius) * (radius + otherRadius)) - displacement.GetLength();
	Vec2 minimumDisplacement = displacement.GetNormalized() * minimumTranslationValue;
	if (out_minimumDisplacement)
	{
		*out_minimumDisplacement = minimumDisplacement;
	}
	center += displacement.GetNormalized() * (minimumTranslationValue / 2.f);
	otherCenter -= displacement.GetNormalized() * (minimumTranslationValue / 2.f);
	return true;
}

bool PushDiscOutOfAABB2D(Vec2& center, float radius, AABB2 const& aabb)
{
	Vec2 nearest = aabb.GetNearestPoint(center);
	if (!DoDiscsOverlap(center, radius, nearest, 0.f))
	{
		return false;
	}

	return PushDiscOutOfPoint2D(center, radius, nearest);
}

IntVec2 GetVec2FlooredToVec2Int(Vec2 const& value)
{
	return IntVec2(static_cast<int>(floor(value.x)), static_cast<int>(floor(value.y)));
}

RaycastResult3D RaycastVsPlane(Vec3 const& start, Vec3 const& direction, float xy, float yz, float xz, Vec3 const& offset , float distance)
{
	RaycastResult3D result;
	Vec3 planeNorm = Vec3(yz, xz, xy);
	Vec3 offsetStart = start - offset;
	if (DotProduct3D(planeNorm, direction) > 0.f)
	{
		result.m_didImpact = false;
		return result;
	}

	float trivector = xy * direction.z + yz * direction.x + xz * direction.y;

	if (abs(trivector) < 0.01f)
	{
		result.m_didImpact = false;
		return result;
	}

	float t = -(xy * offsetStart.z + yz * offsetStart.x + xz * offsetStart.y) / trivector;

	result.m_impactPos.x = start.x + direction.x * t;
	result.m_impactPos.y = start.y + direction.y * t;
	result.m_impactPos.z = start.z + direction.z * t;
	result.m_impactNormal = Vec3(yz, xz, xy);

	if ((result.m_impactPos - start).GetLengthSquared() < distance * distance)
	{
		result.m_impactDist = (result.m_impactPos - start).GetLength();
		result.m_didImpact = true;
	}
	else
	{
		result.m_didImpact = false;
		return result;
	}

	return result;
}

RaycastResult3D RaycastVsCylinderZ3D(Vec3 const& start, Vec3 const& direction, float distance, Vec2 const& center, float minZ, float maxZ, float radius)
{
	RaycastResult3D result;
	result.m_didImpact = false;
	Vec2 start2D = Vec2(start.x, start.y);
	Vec2 fwd2D = Vec2(direction.x, direction.y);
	fwd2D.Normalize();
	Vec3 ray = direction * distance;
	Vec3 end = start + ray;
	Vec2 end2D = Vec2(end.x, end.y);

	RaycastResult2D result2D = RaycastVsDisc2D(start2D, fwd2D, distance, center, radius);

	float t1 = RangeMap(result2D.m_impactPos.x, start.x, end.x, 0.f, 1.f);
	
	if (result2D.m_didImpact)
	{
		float z1 = RangeMap(t1, 0.f, 1.f, start.z, end.z);

		if ((z1 > minZ && z1 < maxZ))
		{
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(result2D.m_impactNormal.x, result2D.m_impactNormal.y, 0.f);
			result.m_impactPos = Vec3(result2D.m_impactPos.x, result2D.m_impactPos.y, z1);
			result.m_impactDist = (result.m_impactPos - start).GetLength();
			return result;
		}
	}

	float tz1 = RangeMap(minZ, start.z, end.z, 0.f, 1.f);
	float tz2 = RangeMap(maxZ, start.z, end.z, 0.f, 1.f);

	Vec2 point = Vec2(RangeMap(tz1, 0.f,1.f,start.x, end.x), RangeMap(tz1, 0.f,1.f, start.y, end.y));

	if (IsPointInDisc(point, center, radius))
	{
		result.m_impactPos = Vec3(point.x, point.y, minZ);
		result.m_didImpact = true;
		result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
		result.m_impactDist = (result.m_impactPos - start).GetLength();
	}

	point = Vec2(RangeMap(tz2, 0.f,1.f,start.x, end.x), RangeMap(tz2, 0.f,1.f, start.y, end.y));
	if (IsPointInDisc(point, center, radius))
	{
		float dist = ( Vec3(point.x, point.y, maxZ) - start).GetLength();
		if ((dist < result.m_impactDist && result.m_didImpact) || !result.m_didImpact)
		{
			result.m_impactPos = Vec3(point.x, point.y, maxZ);
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
			result.m_impactDist = dist;
		}
	}

	return result;
}

RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius)
{
	if (IsPointInDisc(startPos, discCenter, discRadius))
	{
		return { true,0,startPos,-fwdNormal };
	}

	Vec2 displacement = discCenter - startPos;

	float y = DotProduct2D(displacement,fwdNormal.GetRotated90Degrees());
	float x = DotProduct2D(displacement,fwdNormal);

	if (x < 0)
	{
		return {false,0,Vec2(),fwdNormal};
	}

	if (fabs(y) > discRadius)
	{
		return {false,0,Vec2(),fwdNormal};
	}

	float lengthInDisc = sqrt(discRadius * discRadius - y * y);
	float rayLength    = x - (SignFloat(x) * lengthInDisc);

	Vec2 pointOnDisc     = startPos + fwdNormal * rayLength;
	//Vec2 reflectedNormal = fwdNormal.GetReflected((pointOnDisc - discCenter).GetNormalized());

	if (rayLength < maxDist)
	{
		return { true,rayLength,pointOnDisc, (pointOnDisc - discCenter).GetNormalized() };
	}

	return {false,0,Vec2(),-fwdNormal};
}

RaycastResult2D RaycastVsSegment(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 startSeg, Vec2 endSeg)
{
	RaycastResult2D result{};

	Vec2 startToSegStart = startSeg - startPos;
	Vec2 startToSegEnd = endSeg - startPos;
	Vec2 jBasis = fwdNormal.GetRotated90Degrees();

	float startToSegStartProjectedOnJ = DotProduct2D(startToSegStart, jBasis);
	float startToSegEndProjectedOnJ = DotProduct2D(startToSegEnd, jBasis);

	if (startToSegStartProjectedOnJ * startToSegEndProjectedOnJ >= 0.f)
	{
		result.m_didImpact = false;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	float t = startToSegStartProjectedOnJ / (startToSegStartProjectedOnJ - startToSegEndProjectedOnJ);
	Vec2 displacementAlongSegment = endSeg - startSeg;
	Vec2 impactPosition = startSeg + (t * displacementAlongSegment);
	Vec2 displacementAlongRay = impactPosition - startPos;

	if (DotProduct2D(displacementAlongRay, fwdNormal) < 0)
	{
		result.m_didImpact = false;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	float impactDistance = DotProduct2D(fwdNormal,displacementAlongRay);

	if (impactDistance >= maxDist)
	{
		result.m_didImpact = false;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	Vec2 impactNormal;

	if (startToSegEndProjectedOnJ < 0.f)
	{
		impactNormal = displacementAlongSegment.GetRotatedMinus90Degrees().GetNormalized();
	}
	else
	{
		impactNormal = displacementAlongSegment.GetRotated90Degrees().GetNormalized();
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDistance;
	result.m_impactNormal = impactNormal;
	result.m_impactPos = impactPosition;

	return result;
}

RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 const& aabb)
{
	RaycastResult2D result;
	Vec2 displacement = fwdNormal * maxDist;
	Vec2 endPos = startPos + displacement;

	if (aabb.IsPointInside(startPos))
	{
		result.m_didImpact = true;
		result.m_impactPos = startPos;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	float rangeX = endPos.x - startPos.x;
	float rangeY = endPos.y - startPos.y;

	float rangeXScale = 1.f / rangeX;
	float rangeYScale = 1.f / rangeY;

	FloatRange parametricRange = FloatRange(0.f, 1.f);
	FloatRange normalizedRangeAlongX;
	normalizedRangeAlongX.m_min = (aabb.m_min.x - startPos.x) * rangeXScale;
	normalizedRangeAlongX.m_max = (aabb.m_max.x - startPos.x) * rangeXScale;

	FloatRange normalizedRangeAlongY;
	normalizedRangeAlongY.m_min = (aabb.m_min.y - startPos.y) * rangeYScale;
	normalizedRangeAlongY.m_max = (aabb.m_max.y - startPos.y) * rangeYScale;

	if (normalizedRangeAlongY.m_min > normalizedRangeAlongY.m_max)
	{
		std::swap(normalizedRangeAlongY.m_min,normalizedRangeAlongY.m_max);
	}
	if (normalizedRangeAlongX.m_min > normalizedRangeAlongX.m_max)
	{
		std::swap(normalizedRangeAlongX.m_min, normalizedRangeAlongX.m_max);
	}

	bool hit = false;
	Vec2 impactNormal;
	float t = 1.f;
	if (parametricRange.IsOnRange(normalizedRangeAlongX.m_min))
	{
		float contendingT = normalizedRangeAlongX.m_min;
		if (normalizedRangeAlongY.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec2(-1.f, 0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongX.m_max))
	{
		float contendingT = normalizedRangeAlongX.m_max;
		if (normalizedRangeAlongY.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec2(1.f, 0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongY.m_min))
	{
		float contendingT = normalizedRangeAlongY.m_min;
		if (normalizedRangeAlongX.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec2(0.f, -1.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongY.m_max))
	{
		float contendingT = normalizedRangeAlongY.m_max;
		if (normalizedRangeAlongX.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec2(0.f, 1.f);
			}
		}
	}

	if (!hit)
	{
		result.m_didImpact = false;
		return result;
	}

	Vec2 displacementToHitPos = displacement * t;

	result.m_impactDist = displacementToHitPos.GetLength();
	result.m_impactNormal = impactNormal;
	result.m_impactNormal.x *= SignFloat(fwdNormal.x);
	result.m_impactNormal.y *= SignFloat(fwdNormal.y);

	result.m_didImpact = true;
	result.m_impactPos = startPos + displacementToHitPos;

	return result;
}

RaycastResult2D RaycastVsPlane2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Plane2D const& plane)
{
	Vec2 endPos = startPos + fwdNormal * maxDist;
	Vec2 displacement = endPos - startPos;

	Vec2 transStartPos = startPos - plane.m_normal * plane.m_distance;
	Vec2 transEndPos = endPos - plane.m_normal * plane.m_distance;

	float startRej = DotProduct2D(transStartPos, plane.m_normal);
	float endRej = DotProduct2D(transEndPos, plane.m_normal);

	if (startRej * endRej < 0.f)
	{
		float rejectiveDisplacement = endRej - startRej;

		RaycastResult2D result;
		result.m_didImpact = true;
		result.m_impactPos = startPos + displacement * abs(startRej / rejectiveDisplacement);
		result.m_impactDist = (result.m_impactPos - startPos).GetLength();
		result.m_impactNormal = (startRej > 0.f) ? plane.m_normal : -plane.m_normal;
		return result;
	}

	RaycastResult2D result;
	result.m_didImpact = false;

	return result;
}

RaycastResult3D RaycastVsPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3D const& plane)
{
	Vec3 endPos = startPos + fwdNormal * maxDist;
	Vec3 displacement = endPos - startPos;

	Vec3 transStartPos = startPos - plane.m_normal * plane.m_distance;
	Vec3 transEndPos = endPos - plane.m_normal * plane.m_distance;

	float startRej = DotProduct3D(transStartPos, plane.m_normal);
	float endRej = DotProduct3D(transEndPos, plane.m_normal);

	if (startRej * endRej < 0.f)
	{
		float rejectiveDisplacement = endRej - startRej;

		RaycastResult3D result;
		result.m_didImpact = true;
		result.m_impactPos = startPos + displacement * abs(startRej / rejectiveDisplacement);
		result.m_impactDist = (result.m_impactPos - startPos).GetLength();
		result.m_impactNormal = (startRej > 0.f) ? plane.m_normal : -plane.m_normal;
		return result;
	}

	return { false };
}

float NormalizeByte(unsigned char b)
{
	float const scale = 1.f / 255.f;
	return static_cast<float>(b) * scale;
}

unsigned char DenormalizeByte(float f)
{
	if (f < 0.f)
	{
		f = 0.f;
	}
	else if (f > 1.f)
	{
		f = 1.f;
	}

	if (f == 0.f)
	{
		return 0;
	}

	f = (std::ceilf(f * 256.f) - 1.f);

	unsigned char c = static_cast<unsigned char>(f);

	return c;
}

float DotProduct3D(Vec3 const& a, Vec3 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.y - b.x * a.y;
}

Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	Vec3 result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

float DotProduct4D(Vec4 const& a, Vec4 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

bool DoesAABB2OverlapDisc2D(AABB2 const& aabb, Vec2 const& discCenter, float discRadius)
{
	Vec2 nearest = aabb.GetNearestPoint(discCenter);
	return IsPointInDisc(nearest, discCenter, discRadius);
}

bool DoAABBsOverlap3D(AABB3 const& first, AABB3 const& second)
{
	FloatRange xFirst = FloatRange(first.m_mins.x, first.m_maxs.x);
	FloatRange yFirst = FloatRange(first.m_mins.y, first.m_maxs.y);
	FloatRange zFirst = FloatRange(first.m_mins.z, first.m_maxs.z);

	FloatRange xSecond = FloatRange(second.m_mins.x, second.m_maxs.x);
	FloatRange ySecond = FloatRange(second.m_mins.y, second.m_maxs.y);
	FloatRange zSecond = FloatRange(second.m_mins.z, second.m_maxs.z);

	if (!xFirst.IsOverlappingWith(xSecond))
	{
		return false;
	}

	if (!yFirst.IsOverlappingWith(ySecond))
	{
		return false;
	}

	if (!zFirst.IsOverlappingWith(zSecond))
	{
		return false;
	}

	return true;
}

bool DoSpheresOverlap3D(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	Vec3 displacement = centerB - centerA;

	if (displacement.GetLengthSquared() < (radiusA + radiusB) * (radiusA + radiusB))
	{
		return true;
	}

	return false;
}

bool DoZCylindersOverlap3D(Vec2 cylinderACenterXY, float cylinderARadius, FloatRange cylinderAMinMaxZ, Vec2 cylinderBCenterXY, float cylinderBRadius, FloatRange cylinderBMinMaxZ)
{
	bool m_overlapZs = cylinderAMinMaxZ.IsOverlappingWith(cylinderBMinMaxZ);
	bool m_overlapDiscs = DoDiscsOverlap(cylinderACenterXY,cylinderARadius,cylinderBCenterXY,cylinderBRadius);
	return m_overlapZs && m_overlapDiscs;
}

bool DoPlaneAndOBBOverlap3D(Plane3D const& plane, OBB3 const& obb3)
{
	std::array<Vec3, 8> obbCorners = obb3.GetCorners();
	bool IsInFront = false;
	bool IsBehind = false;

	for (Vec3& corner : obbCorners)
	{
		if (plane.IsPointInFrontOf(corner))
		{
			IsInFront = true;
		}
		else
		{
			IsBehind = true;
		}

		if (IsInFront && IsBehind)
		{
			return true;
		}
	}

	return false;
}

bool DoSphereAndAABBOverlap3D(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& box)
{
	Vec3 point = GetNearestPointOnAABB3(sphereCenter, box);

	return DoSpheresOverlap3D(sphereCenter,sphereRadius,point, 0.f);
}

bool DoZCylinderAndAABBOverlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, FloatRange const& cylinderMinMaxZ, AABB3 const& box)
{
	Vec3 aabbCenter = box.GetCenter();
	Vec3 nearestOnCylinder = GetNearestPointOnZCylinder(aabbCenter, cylinderCenterXY, cylinderMinMaxZ,cylinderRadius);

	return box.IsPointInside(nearestOnCylinder);
}

bool DoZCylinderAndSphereOverlap3D(Vec2 const& centerXY, float cylinderRadius, FloatRange cylinderMinMaxZ, Vec3 sphereCenter, float sphereRadius)
{
	Vec3 nearestOnCylinder = GetNearestPointOnZCylinder(sphereCenter, centerXY, cylinderMinMaxZ, cylinderRadius);

	return DoSpheresOverlap3D(sphereCenter,sphereRadius,nearestOnCylinder,0.f);
}

RaycastResult3D RaycastVsSphere3D(Vec3 rayStart, Vec3 rayFwdNormal, float rayLength, const Vec3& sphereCenter, float sphereRadius)
{
	Vec3 displacement = sphereCenter - rayStart;
	float distanceAlongRay = DotProduct3D(displacement, rayFwdNormal);

	if (distanceAlongRay < 0 || distanceAlongRay > rayLength) 
	{
		return { false };
	}

	Vec3 rejection = displacement - GetProjectedOnto3D(displacement, rayFwdNormal);

	if (rejection.GetLengthSquared() <= sphereRadius * sphereRadius) 
	{
		float baseProjection = sqrt(sphereRadius * sphereRadius - rejection.GetLengthSquared());
		Vec3 pos = sphereCenter - rejection - baseProjection * rayFwdNormal;
		Vec3 normal = (pos - sphereCenter).GetNormalized();
		
		RaycastResult3D result;
		result.m_didImpact = true;
		result.m_impactDist = (pos - rayStart).GetLength();
		result.m_impactPos = pos;
		result.m_impactNormal = normal;
		return result;
	}

	return { false };
}

RaycastResult3D RaycastVsAABB3D(Vec3 rayStart, Vec3 rayFwdNormal, float rayLength, AABB3 const& aabb)
{
	RaycastResult3D result;
	Vec3 displacement = rayFwdNormal * rayLength;
	Vec3 endPos = rayStart + displacement;

	if (aabb.IsPointInside(rayStart))
	{
		result.m_didImpact = true;
		result.m_impactPos = rayStart;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -rayFwdNormal;
		return result;
	}

	float rangeX = endPos.x - rayStart.x;
	float rangeY = endPos.y - rayStart.y;
	float rangeZ = endPos.z - rayStart.z;
	float rangeXScale = 1.f / rangeX;
	float rangeYScale = 1.f / rangeY;
	float rangeZScale = 1.f / rangeZ;

	FloatRange parametricRange = FloatRange(0.f, 1.f);
	FloatRange normalizedRangeAlongX;
	normalizedRangeAlongX.m_min = (aabb.m_mins.x - rayStart.x) * rangeXScale;
	normalizedRangeAlongX.m_max = (aabb.m_maxs.x - rayStart.x) * rangeXScale;

	FloatRange normalizedRangeAlongY;
	normalizedRangeAlongY.m_min = (aabb.m_mins.y - rayStart.y) * rangeYScale;
	normalizedRangeAlongY.m_max = (aabb.m_maxs.y - rayStart.y) * rangeYScale;

	FloatRange normalizedRangeAlongZ;
	normalizedRangeAlongZ.m_min = (aabb.m_mins.z - rayStart.z) * rangeZScale;
	normalizedRangeAlongZ.m_max = (aabb.m_maxs.z - rayStart.z) * rangeZScale;

	if (normalizedRangeAlongY.m_min > normalizedRangeAlongY.m_max)
	{
		std::swap(normalizedRangeAlongY.m_min, normalizedRangeAlongY.m_max);
	}
	if (normalizedRangeAlongX.m_min > normalizedRangeAlongX.m_max)
	{
		std::swap(normalizedRangeAlongX.m_min, normalizedRangeAlongX.m_max);
	}
	if (normalizedRangeAlongZ.m_min > normalizedRangeAlongZ.m_max)
	{
		std::swap(normalizedRangeAlongZ.m_min, normalizedRangeAlongZ.m_max);
	}

	bool hit = false;
	Vec3 impactNormal;
	float t = 1.f;

	if (parametricRange.IsOnRange(normalizedRangeAlongX.m_min))
	{
		float contendingT = normalizedRangeAlongX.m_min;
		if (normalizedRangeAlongY.IsOnRange(contendingT) && normalizedRangeAlongZ.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(-1.f, 0.f,0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongX.m_max))
	{
		float contendingT = normalizedRangeAlongX.m_max;
		if (normalizedRangeAlongY.IsOnRange(contendingT) && normalizedRangeAlongZ.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(1.f, 0.f,0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongY.m_min))
	{
		float contendingT = normalizedRangeAlongY.m_min;
		if (normalizedRangeAlongX.IsOnRange(contendingT) && normalizedRangeAlongZ.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(0.f, -1.f,0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongY.m_max))
	{
		float contendingT = normalizedRangeAlongY.m_max;
		if (normalizedRangeAlongX.IsOnRange(contendingT) && normalizedRangeAlongZ.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(0.f, 1.f,0.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongZ.m_min))
	{
		float contendingT = normalizedRangeAlongZ.m_min;
		if (normalizedRangeAlongX.IsOnRange(contendingT) && normalizedRangeAlongY.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(0.f, 0.f, -1.f);
			}
		}
	}
	if (parametricRange.IsOnRange(normalizedRangeAlongZ.m_max))
	{
		float contendingT = normalizedRangeAlongZ.m_max;
		if (normalizedRangeAlongX.IsOnRange(contendingT) && normalizedRangeAlongY.IsOnRange(contendingT))
		{
			hit = true;
			if (contendingT < t)
			{
				t = contendingT;
				impactNormal = Vec3(0.f, 0.f, 1.f);
			}
		}
	}
	

	if (!hit)
	{
		result.m_didImpact = false;
		return result;
	}

	Vec3 displacementToHitPos = displacement * t;

	result.m_impactDist = displacementToHitPos.GetLength();
	result.m_impactNormal = impactNormal;
	result.m_impactNormal.x *= SignFloat(rayFwdNormal.x);
	result.m_impactNormal.y *= SignFloat(rayFwdNormal.y);
	result.m_impactNormal.z *= SignFloat(rayFwdNormal.z);

	result.m_didImpact = true;
	result.m_impactPos = rayStart + displacementToHitPos;

	return result;
}

float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	float AB = Interpolate(A, B, t);
	float BC = Interpolate(B, C, t);
	float CD = Interpolate(C, D, t);

	float ABC = Interpolate(AB, BC, t);
	float BCD = Interpolate(BC, CD, t);

	return Interpolate(ABC,BCD,t);
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	float AB = Interpolate(A, B, t);
	float BC = Interpolate(B, C, t);
	float CD = Interpolate(C, D, t);
	float DE = Interpolate(D, E, t);
	float EF = Interpolate(E, F, t);

	float ABC = Interpolate(AB, BC, t);
	float BCD = Interpolate(BC, CD, t);
	float CDE = Interpolate(CD, DE, t);
	float DEF = Interpolate(DE, EF, t);

	float ABCD = Interpolate(ABC, BCD, t);
	float BCDE = Interpolate(BCD, CDE, t);
	float CDEF = Interpolate(CDE, DEF, t);

	float ABCDE = Interpolate(ABCD, BCDE, t);
	float BCDEF = Interpolate(BCDE, CDEF, t);

	return Interpolate(ABCDE, BCDEF, t);
}

Vec2 SphericalInterpolate2D(Vec2 start, Vec2 end, float t)
{
	float deg = GetAngleDegreesBetweenVectors2D(start, end);
	return (CosDegrees(deg * t) / CosDegrees(deg)) * start + (CosDegrees(deg * (1.f - t)) / CosDegrees(deg)) * end;
}
