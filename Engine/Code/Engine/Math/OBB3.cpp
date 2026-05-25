#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"

OBB3::OBB3(AABB3 const& copy)
	: m_center(copy.GetCenter())
{
	Vec3 dims = copy.m_maxs - copy.m_mins;
	m_halfHeight = std::abs(dims.z * 0.5f);
	m_halfWidth = std::abs(dims.x * 0.5f);
	m_halfLength = std::abs(dims.y * 0.5f);

	Mat44 rot = Mat44::IDENTITY;
	m_iBasis = rot.GetIBasis3D();
	m_jBasis = rot.GetJBasis3D();
	m_kBasis = rot.GetKBasis3D();
}

OBB3::OBB3(Vec3 center, float width, float length, float height, EulerAngles orientation)
	: m_center(center)
	, m_halfHeight(std::abs(height * 0.5f))
	, m_halfLength(std::abs(length * 0.5f))
	, m_halfWidth(std::abs(width * 0.5f))
{
	Mat44 rot = orientation.GetAsMatrix_IFwd_JLeft_KUp();
	m_iBasis = rot.GetIBasis3D();
	m_jBasis = rot.GetJBasis3D();
	m_kBasis = rot.GetKBasis3D();
}

RaycastResult3D OBB3::GetRaycastResult(Vec3 const& rayStart, Vec3 const& rayFwdNormal, float rayLength)
{
	Mat44 mat = Mat44(m_iBasis, m_jBasis, m_kBasis, Vec3(0.f, 0.f, 0.f));
	Mat44 matInv = mat.GetOrthonormalInverse();

	Vec3 tRayStart = matInv.TransformPosition3D(rayStart - m_center);
	Vec3 tRayFwdNormal = matInv.TransformPosition3D(rayFwdNormal);

	AABB3 abb = AABB3(Vec3(-m_halfWidth, -m_halfLength, -m_halfHeight), Vec3(m_halfWidth, m_halfLength, m_halfHeight));

	RaycastResult3D result = RaycastVsAABB3D(tRayStart, tRayFwdNormal, rayLength, abb);
	result.m_impactPos = mat.TransformPoint(result.m_impactPos) + m_center;
	result.m_impactNormal = mat.TransformPoint(result.m_impactNormal);
	return result;
}

void OBB3::Translate(Vec3 const& translation)
{
	m_center += translation;
}

bool OBB3::IntersectsZCylinder( Vec2 const& base, FloatRange heightRange, float radius)
{
	std::array<Vec3, 8> myCorners = GetCorners();

	bool first = true;
	FloatRange myRangeOnK;

	for (Vec3 const& corner : myCorners)
	{
		float kProj = DotProduct3D(m_kBasis,  corner);

		//Initialize ranges if this is the first iteration.
		if (first)
		{
			myRangeOnK = FloatRange(kProj, kProj);
			first	   = false;
		}
		else
		{
			if (kProj < myRangeOnK.m_min)
			{
				myRangeOnK.m_min = kProj;
			}
			if (kProj > myRangeOnK.m_max)
			{
				myRangeOnK.m_max = kProj;
			}
		}
	}

	if (!heightRange.IsOverlappingWith(myRangeOnK))
	{
		return false;
	}

	//Check each points distance from center of disk if it is inside we are intersecting
	for (Vec3 const& corner : myCorners)
	{
		Vec2 cornerXY = Vec2(corner.x, corner.y);
		if (IsPointInDisc(cornerXY, base, radius))
		{
			return true;
		}

	}

	return false;
}

bool OBB3::IntersectsSphere(Vec3 const& center, float radius)
{
	Vec3 nearest = GetNearestPoint(center);
	return DoSpheresOverlap3D(center, radius, nearest, 0.f);
}

bool OBB3::IntersectsOBB3(OBB3 const& obb3)
{
	return OBB3VsOBB3IntersectionTest(obb3) && obb3.OBB3VsOBB3IntersectionTest(*this);
}

bool OBB3::IntersectsABB3(AABB3 const& abb3)
{
	OBB3 obb(abb3);

	return IntersectsOBB3(obb);
}

bool OBB3::IsPointInside(Vec3 const& point) const
{
	float dotI = DotProduct3D(point - m_center, GetIBasis());
	if (dotI > m_halfWidth || dotI < m_halfLength)
	{
		return false;
	}

	float dotJ = DotProduct3D(point - m_center, GetJBasis());
	if (dotJ > m_halfLength || dotJ < m_halfLength)
	{
		return false;
	}

	float dotK = DotProduct3D(point - m_center, GetKBasis());
	if(dotK > m_halfHeight || dotK < m_halfHeight)
	{
		return false;
	}

	return true;
}

Vec3 const OBB3::GetNearestPoint(Vec3 const& point) const
{
	float pointJ = DotProduct3D(m_jBasis, point - m_center);
	float pointI = DotProduct3D(m_iBasis, point - m_center);
	float pointK = DotProduct3D(m_kBasis, point - m_center);

	Vec3 pointIJK = Vec3(pointI, pointJ, pointK);
	if (pointIJK.x > m_halfWidth)
	{
		pointIJK.x = m_halfWidth;
	}
	else if (pointIJK.x < -m_halfWidth)
	{
		pointIJK.x = -m_halfWidth;
	}

	if (pointIJK.y > m_halfLength)
	{
		pointIJK.y = m_halfLength;
	}
	else if (pointIJK.y < -m_halfLength)
	{
		pointIJK.y = -m_halfLength;
	}

	if (pointIJK.z > m_halfHeight)
	{
		pointIJK.z = m_halfHeight;
	}
	else if(pointIJK.z < -m_halfHeight)
	{
		pointIJK.z = -m_halfHeight;
	}

	Vec3 result = m_iBasis * pointIJK.x + m_jBasis * pointIJK.y + m_kBasis * pointIJK.z;

	return result + m_center;
}

void OBB3::SetCenter(Vec3 const& center)
{
	m_center = center;
}

void OBB3::SetRotation(EulerAngles const& angles)
{
	Mat44 rot = angles.GetAsMatrix_IFwd_JLeft_KUp();
	m_iBasis = rot.GetIBasis3D();
	m_jBasis = rot.GetJBasis3D();
	m_kBasis = rot.GetKBasis3D();
}

void OBB3::Rotate(EulerAngles const& angles)
{
	Mat44 rot = angles.GetAsMatrix_IFwd_JLeft_KUp();
	m_iBasis = rot.TransformPoint(m_iBasis);
	m_jBasis = rot.TransformPoint(m_jBasis);
	m_kBasis = rot.TransformPoint(m_kBasis);
}

EulerAngles OBB3::GetOrientationDegrees() const
{
	return EulerAngles::MakeFromNormal_IFwd_JLeft_KUp(m_iBasis);
}

std::array<Vec3, 8> OBB3::GetCorners() const
{
	std::array<Vec3, 8> corners;

	corners[0] = m_center + m_iBasis * m_halfWidth + m_jBasis * m_halfLength + m_kBasis * m_halfHeight;
	corners[1] = m_center + m_iBasis * m_halfWidth + m_jBasis * m_halfLength - m_kBasis * m_halfHeight;
	corners[2] = m_center + m_iBasis * m_halfWidth - m_jBasis * m_halfLength + m_kBasis * m_halfHeight;
	corners[3] = m_center + m_iBasis * m_halfWidth - m_jBasis * m_halfLength - m_kBasis * m_halfHeight;

	corners[4] = m_center - m_iBasis * m_halfWidth + m_jBasis * m_halfLength + m_kBasis * m_halfHeight;
	corners[5] = m_center - m_iBasis * m_halfWidth + m_jBasis * m_halfLength - m_kBasis * m_halfHeight;
	corners[6] = m_center - m_iBasis * m_halfWidth - m_jBasis * m_halfLength + m_kBasis * m_halfHeight;
	corners[7] = m_center - m_iBasis * m_halfWidth - m_jBasis * m_halfLength - m_kBasis * m_halfHeight;

	return corners;
}

Vec3 const OBB3::GetIBasis() const
{
	return m_iBasis;
}

Vec3 const OBB3::GetJBasis() const
{
	return m_jBasis;
}

Vec3 const OBB3::GetKBasis() const
{
	return m_kBasis;
}

bool OBB3::OBB3VsOBB3IntersectionTest(OBB3 const& obb3) const
{
	std::array<Vec3, 8> otherCorners = obb3.GetCorners();

	bool first = true;

	FloatRange myRangeOnI = FloatRange(-m_halfWidth, m_halfWidth);
	FloatRange myRangeOnJ = FloatRange(-m_halfLength, m_halfLength);
	FloatRange myRangeOnK = FloatRange(-m_halfHeight, m_halfHeight);

	FloatRange rangeOnI;
	FloatRange rangeOnJ;
	FloatRange rangeOnK;

	for (Vec3 const& corner : otherCorners)
	{
		Vec3 centeredCorner = corner - m_center;
		float iProj = DotProduct3D(m_iBasis, centeredCorner);

		//Initialize ranges if this is the first iteration.
		if (first)
		{
			rangeOnI = FloatRange(iProj, iProj);
			first = false;
		}
		else
		{
			if (iProj < rangeOnI.m_min)
			{
				rangeOnI.m_min = iProj;
			}
			if (iProj > rangeOnI.m_max)
			{
				rangeOnI.m_max = iProj;
			}
		}
	}
	if (!rangeOnI.IsOverlappingWith(myRangeOnI))
	{
		return false;
	}

	first = true;
	for (Vec3 const& corner : otherCorners)
	{
		Vec3 centeredCorner = corner - m_center;
		float jProj = DotProduct3D(m_jBasis, centeredCorner);

		//Initialize ranges if this is the first iteration.
		if (first)
		{
			rangeOnJ = FloatRange(jProj, jProj);
			first = false;
		}
		else
		{
			if (jProj < rangeOnJ.m_min)
			{
				rangeOnJ.m_min = jProj;
			}
			if (jProj > rangeOnJ.m_max)
			{
				rangeOnJ.m_max = jProj;
			}
		}
	}
	if (!rangeOnJ.IsOverlappingWith(myRangeOnJ))
	{
		return false;
	}

	first = true;
	for (Vec3 const& corner : otherCorners)
	{
		Vec3 centeredCorner = corner - m_center;
		float kProj = DotProduct3D(m_kBasis, centeredCorner);

		//Initialize ranges if this is the first iteration.
		if (first)
		{
			rangeOnK = FloatRange(kProj, kProj);
			first = false;
		}
		else
		{
			if (kProj < rangeOnK.m_min)
			{
				rangeOnK.m_min = kProj;
			}
			if (kProj > rangeOnK.m_max)
			{
				rangeOnK.m_max = kProj;
			}
		}
	}
	if (!rangeOnK.IsOverlappingWith(myRangeOnK))
	{
		return false;
	}

	return true;
}
