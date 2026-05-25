#include "AABB3.hpp"

AABB3::AABB3()
	: m_mins(Vec3(0.f, 0.f, 0.f))
	, m_maxs(Vec3(0.f, 0.f, 0.f))
{
}

AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{

}

AABB3::AABB3(float minx, float miny, float minz, float maxx, float maxy, float maxz)
	: m_mins(Vec3(minx,miny,minz))
	, m_maxs(Vec3(maxx,maxy,maxz))
{

}

std::vector<Vec3> AABB3::GetCorners() const
{
	std::vector<Vec3> corners;
	corners.emplace_back(m_maxs.x, m_maxs.y, m_maxs.z);
	corners.emplace_back(m_mins.x, m_maxs.y, m_maxs.z);
	corners.emplace_back(m_maxs.x, m_mins.y, m_maxs.z);
	corners.emplace_back(m_mins.x, m_mins.y, m_maxs.z);
	corners.emplace_back(m_maxs.x, m_maxs.y, m_mins.z);
	corners.emplace_back(m_mins.x, m_maxs.y, m_mins.z);
	corners.emplace_back(m_maxs.x, m_mins.y, m_mins.z);
	corners.emplace_back(m_mins.x, m_mins.y, m_mins.z);

	return corners;
}

bool AABB3::IsPointInside(Vec3 const& point) const
{
	if ( point.x < m_mins.x || point.x > m_maxs.x )
	{
		return false;
	}

	if ( point.y < m_mins.y || point.y > m_maxs.y )
	{
		return false;
	}

	if ( point.z < m_mins.z || point.z > m_maxs.z )
	{
		return false;
	}

	return true;
}

void AABB3::Translate(Vec3 const& translation)
{
	m_mins += translation;
	m_maxs += translation;
}

Vec3 AABB3::GetCenter() const
{
	Vec3 center = ((m_maxs + m_mins) * 0.5f);
	return center;
}

AABB3 AABB3::GetTranslated(Vec3 const& translation)
{
	AABB3 copy = *this;
	copy.Translate(translation);
	return copy;
}
