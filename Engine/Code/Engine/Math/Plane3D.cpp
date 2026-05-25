#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/MathUtils.hpp"

Plane3D::~Plane3D()
{

}

Plane3D::Plane3D(Vec3 normal, float distance /*= 0.f*/)
	: m_normal(normal)
	, m_distance(distance)
{

}

void Plane3D::Normalize()
{
	m_normal.Normalize();
}

EulerAngles Plane3D::GetOrientation() const
{
	return EulerAngles::MakeFromNormal_IFwd_JLeft_KUp(m_normal);
}

Plane3D Plane3D::GetNormalized() const
{
	Plane3D result = *this;
	result.Normalize();
	return result;
}

float Plane3D::MagnitudeSquared() const
{
	return m_normal.GetLengthSquared();
}

bool Plane3D::IsPointInFrontOf(Vec3 const& point) const
{
	Vec3 translatedPoint = point - m_normal * m_distance;
	return DotProduct3D(translatedPoint, m_normal) > 0.f;
}

Vec3 Plane3D::ProjectPointOntoPlane(Vec3 const& point) const
{
	Vec3 result = GetProjectedOnto3D(point, m_normal);
	result = point - result;

	return result + m_normal * m_distance;
}

Vec3 Plane3D::GetTangentZUp() const
{
	Vec3 up = Vec3(0.f, 0.f, 1.f);
	if (abs((m_normal - up).GetLengthSquared()) < 0.00000001f)
	{
		Vec3 left = Vec3(0.f, 1.f, 0.f);
		Vec3 norm = m_normal.GetNormalized();

		return CrossProduct3D(norm,CrossProduct3D(norm, up)).GetNormalized();
	}
	else
	{
		Vec3 norm = m_normal.GetNormalized();
		return CrossProduct3D(norm, up).GetNormalized();
	}

}

Vec3 Plane3D::GetBitangentZUp() const
{
	Vec3 norm = m_normal.GetNormalized();
	return CrossProduct3D(norm, GetTangentZUp()).GetNormalized();
}