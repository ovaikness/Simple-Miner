#pragma once
#include "RaycastResult.hpp"
#include "Engine/Math/FloatRange.hpp"

struct AABB3;
struct AABB2;
struct IntVec2;
struct Vec2;
struct Vec3;
struct Vec4;

class Capsule2;
class OBB2;
class OBB3;
class Plane3D;
class Plane2D;
class LineSegment2;

constexpr float M_PI = 3.1415925f;

float GetDistance2D(Vec2 start, Vec2 end);
float GetDistanceSquared2D(Vec2 start, Vec2 end);
float GetDistanceXY3D(Vec3 start, Vec3 end);
float GetDistanceXYSquared3D(Vec3 start, Vec3 end);
float GetDistance3D(Vec3 start, Vec3 end);
float GetDistanceSquared3D(Vec3 start, Vec3 end);

float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);

float CosDegrees(float degrees);
float SinDegrees(float degrees);
float TanDegrees(float degrees);
float Atan2Degrees(float Y, float X);

bool IsPointInDisc(Vec2 const& point, Vec2 const& center, float radius);
bool DoDiscsOverlap(Vec2 position, float radius, Vec2 otherPosition, float otherRadius);
bool DoSpheresOverlap(Vec3 position, float radius, Vec3 otherPosition, float otherRadius);

void TransformPosition2D(Vec2& position, float scale, float rotation, Vec2 const& translation);
void TransformPositionXY3D(Vec3& position, float scale, float rotation, Vec2 const& translation);

void TransformPosition2D(Vec2& position, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void TransformPositionXY3D(Vec3& position, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);

float WrapDiscInRange(float position, float radius, float min, float max);
float GetFractionWithinRange(float start, float end, float value);
float RangeMap(float value, float startOrigin, float endOrigin, float startDestination, float endDestination);
float RangeMapClamped(float value, float startOrigin, float endOrigin, float startDestination, float endDestination);
float MinFloat(float a, float b);
float MaxFloat(float a, float b);
float SignFloat(float a);
float GetClamped(float value, float min, float max);
float GetClampedZeroToOne(float value);
int   RoundDownToInt(float value);
float GetShortestAngularDispDegrees(float angle, float targetAngle);
float GetTurnedTowardDegrees(float angle, float targetAngle, float amount);

float DotProduct2D(Vec2 const& a, Vec2 const& b);

float GetProjectedLength2D(Vec2 const& a, Vec2 const& b);
Vec2  GetProjectedOnto2D(Vec2 const& a, Vec2 const& b);
Vec3  GetProjectedOnto3D(Vec3 const& a, Vec3 const& b);
float GetRejectedLength2D(Vec2 const& a, Vec2 const& b);
Vec2  GetRejectedOnto2D(Vec2 const& a, Vec2 const& b);

Vec3 GetRejectedOnto3D(Vec3 const& a, Vec3 const& b);

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);

int  GetTaxicabDistance2D(IntVec2 const& a, IntVec2 const& b);
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float degreesOrientation, float degreesAperture, float radius );
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& direction, float degreesAperture, float radius);

bool IsPointInsideHex2D(Vec2 const& point, Vec2 const& center, float radius);

Vec2 GetNearestPointOnInfiniteLine(Vec2 const& point, LineSegment2 const& lineSeg);
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, LineSegment2 const& lineSeg);

Vec3 GetNearestPointOnLineSegment3D(Vec3 const& point, Vec3 const& start, Vec3 const& end);

Vec3 GetNearestPointOnAABB3(Vec3 const& point, AABB3 const& aabb);
Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& obb);

Vec3 GetNearestPointOnZCylinder(Vec3 const& point, Vec2 const& base, FloatRange zHeight, float radius);
Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Capsule2 const& capsule);

bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule);
bool IsPointInsideOBB2(Vec2 const& point, OBB2 const& obb);

Vec3 GetNearestPointOnSphere3D(Vec3 const& point, Vec3 const& center, float radius);
Vec2 GetNearestPointOnDisc2D(Vec2 const& point, Vec2 const& center, float radius);

bool PushDiscOutOfCapsule2D(Vec2& center, float radius, Capsule2 const& capsule, Vec2* out_minimumDisplacement = nullptr);
bool PushDiscOutOfOBB2D(Vec2& center, float radius, OBB2 const& obb2, Vec2* out_minimumDisplacement = nullptr);
bool PushDiscOutOfPoint2D(Vec2& center, float radius, Vec2 const& point, Vec2* out_minimumDisplacement = nullptr);
bool PushDiscOutOfDisc2D(Vec2& center, float radius, Vec2 const& otherCenter, float otherRadius, Vec2* out_minimumDisplacement = nullptr);

bool PushDiscsOutOfEachOther2D(Vec2& center, float radius, Vec2& otherCenter, float otherRadius, Vec2* out_minimumDisplacement = nullptr);
bool PushDiscOutOfAABB2D(Vec2& center, float radius, AABB2 const& aabb);

IntVec2 GetVec2FlooredToVec2Int(Vec2 const& value);

RaycastResult3D RaycastVsPlane(
	Vec3 const& start, Vec3 const& direction,
	float xy, float yz, float xz, Vec3 const& offset, float distance
);

RaycastResult3D RaycastVsCylinderZ3D(
	Vec3 const& start, Vec3 const& direction, float distance,
	Vec2 const& center, float minZ, float maxZ, float radius
);
RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius );
RaycastResult2D RaycastVsSegment(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 startSeg, Vec2 endSeg);
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 const& aabb);
RaycastResult2D RaycastVsPlane2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Plane2D const& plane);
RaycastResult3D RaycastVsPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3D const& plane);
float NormalizeByte(unsigned char b);
unsigned char DenormalizeByte(float f);

float DotProduct3D(Vec3 const& a, Vec3 const& b);
float CrossProduct2D(Vec2 const& a, Vec2 const& b);
Vec3  CrossProduct3D(Vec3 const& a, Vec3 const& b);

float DotProduct4D(Vec4 const& a, Vec4 const& b);

bool DoesAABB2OverlapDisc2D(AABB2 const& aabb, Vec2 const& discCenter, float discRadius);
bool DoAABBsOverlap3D(AABB3 const& first, AABB3 const& second);
bool DoSpheresOverlap3D(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoZCylindersOverlap3D(
	Vec2 cylinderACenterXY,
	float cylinderARadius,
	FloatRange cylinderAMinMaxZ,
	Vec2 cylinderBCenterXY,
	float cylinderBRadius,
	FloatRange cylinderBMinMaxZ
);

bool DoPlaneAndOBBOverlap3D(Plane3D const& plane, OBB3 const& obb3);

bool DoSphereAndAABBOverlap3D(Vec3 const& sphereCenter,
	float sphereRadius,
	AABB3 const& box);
bool DoZCylinderAndAABBOverlap3D(Vec2 const& cylinderCenterXY,
	float cylinderRadius, FloatRange const& cylinderMinMaxZ,
	AABB3 const& box);
bool DoZCylinderAndSphereOverlap3D(
	Vec2 const& centerXY,
	float cylinderRadius,
	FloatRange cylinderMinMaxZ,
	Vec3 sphereCenter,
	float sphereRadius
);

RaycastResult3D RaycastVsSphere3D(Vec3 rayStart, Vec3 rayFwdNormal, float rayLength,
	Vec3 const& sphereCenter, float sphereRadius);

RaycastResult3D RaycastVsAABB3D(Vec3 rayStart, Vec3 rayFwdNormal, float rayLength,
	AABB3 const& aabb);

float ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);

Vec2 SphericalInterpolate2D(Vec2 start, Vec2 end, float t);

template<typename T>
T ComputeCubicBezier(T A, T B, T C, T D, float t)
{
	T AB = Interpolate(A, B, t);
	T BC = Interpolate(B, C, t);
	T CD = Interpolate(C, D, t);

	T ABC = Interpolate(AB, BC, t);
	T BCD = Interpolate(BC, CD, t);

	return Interpolate(ABC, BCD, t);
}

template<typename T>
T ComputeQuinticBezier(T A, T B, T C, T D, T E, T F, float t)
{
	T AB = Interpolate(A, B, t);
	T BC = Interpolate(B, C, t);
	T CD = Interpolate(C, D, t);
	T DE = Interpolate(D, E, t);
	T EF = Interpolate(E, F, t);

	T ABC = Interpolate(AB, BC, t);
	T BCD = Interpolate(BC, CD, t);
	T CDE = Interpolate(CD, DE, t);
	T DEF = Interpolate(DE, EF, t);

	T ABCD = Interpolate(ABC, BCD, t);
	T BCDE = Interpolate(BCD, CDE, t);
	T CDEF = Interpolate(CDE, DEF, t);

	T ABCDE = Interpolate(ABCD, BCDE, t);
	T BCDEF = Interpolate(BCDE, CDEF, t);

	return Interpolate(ABCDE, BCDEF, t);
}

template<typename T>
T Interpolate(T start, T end, float t)
{
	return T(start * (1.f - t) + end * t);
}
template <typename T>
T SmoothStart2(T start, T end, float t)
{
	float tPow = t * t;
	return T(start * (1.f - tPow) + end * tPow);
}

template <typename T>
T SmoothStart3(T start, T end, float t)
{
	float tPow = t * t * t;
	return T(start * (1.f - tPow) + end * tPow);
}
template <typename T>
T SmoothStart4(T start, T end, float t)
{
	float tPow = t * t * t * t;
	return T(start * (1.f - tPow) + end * tPow);
}
template <typename T>
T SmoothStart5(T start, T end, float t)
{
	float tPow = t * t * t * t * t;
	return T((start * (1.f - tPow)) + (end * tPow));
}
template <typename T>
T SmoothStart6(T start, T end, float t)
{
	float tPow = t * t * t * t * t * t;
	return T(start * (1.f - tPow) + end * tPow);
}

template <typename T>
T SmoothStop2(T start, T end, float t)
{
	float invT = 1.f - t;
	float tPow = 1.f - invT * invT;
	return T(start * (1.f - tPow) + end * tPow);
}

template <typename T>
T SmoothStop3(T start, T end, float t)
{
	float invT = 1.f - t;
	float tPow = 1.f - invT * invT * invT;
	return T(start * (1.f - tPow) + end * tPow);
}
template <typename T>
T SmoothStop4(T start, T end, float t)
{
	float invT = 1.f - t;
	float tPow = 1.f - invT * invT * invT * invT;
	return T(start * (1.f - tPow) + end * tPow);
}
template <typename T>
T SmoothStop5(T start, T end, float t)
{
	float invT = 1.f - t;
	float tPow = 1.f - invT * invT * invT * invT * invT;
	return T(start * (1.f - tPow) + end * tPow);
}
template <typename T>
T SmoothStop6(T start, T end, float t)
{
	float invT = 1.f - t;
	float tPow = 1.f - invT * invT * invT * invT * invT * invT;
	return T(start * (1.f - tPow) + end * tPow);
}

template <typename T>
T SmoothStep3(T start, T end, float t)
{
	return Interpolate(SmoothStart2(start, end, t), SmoothStop2(start, end, t), t);
}

template <typename T>
T SmoothStep5(T start, T end, float t)
{
	float tBezier = ComputeQuinticBezier1D(0.f, 0.f, 0.f, 1.f, 1.f, 1.f, t);
	return Interpolate(start,end,tBezier);
}

template <typename T>
T Hesitate3(T start, T end, float t)
{
	float tBezier = ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);
	return Interpolate(start, end, tBezier);
}

template<typename T>
T Hesitate5(T start, T end, float t)
{
	float tBezier = ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f,1.f, t);
	return Interpolate(start, end, tBezier);
}