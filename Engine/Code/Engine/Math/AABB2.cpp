#include "AABB2.hpp"
#include "MathUtils.hpp"
AABB2::~AABB2()
{
}
AABB2::AABB2()
{
}
AABB2::AABB2(AABB2 const& copyFrom)
    : m_min(copyFrom.m_min)
    , m_max(copyFrom.m_max)
{
}

AABB2::AABB2(float minx, float miny, float maxx, float maxy)
    : m_min(Vec2(minx,miny))
    , m_max(Vec2(maxx,maxy))
{
}

AABB2::AABB2(Vec2 const& min, Vec2 const& max)
    : m_min(min)
    , m_max(max)
{
}

bool AABB2::IsPointInside(Vec2 const& point) const
{
    if (point.x > m_min.x && point.x < m_max.x && point.y > m_min.y && point.y < m_max.y)
    {
        return true;
    }
    return false;
}

Vec2 const AABB2::GetCenter() const
{
    return Vec2((m_max.x+m_min.x)/2.f,(m_max.y + m_min.y)/2.f);
}

Vec2 const AABB2::GetDimensions() const
{
    return m_max - m_min;
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& position) const
{
    Vec2 nearestPoint = position;

    if (nearestPoint.x < m_min.x)
    {
        nearestPoint.x = m_min.x;
    }
    if (nearestPoint.x > m_max.x)
    {
        nearestPoint.x = m_max.x;
    }
    if (nearestPoint.y < m_min.y)
    {
        nearestPoint.y = m_min.y;
    }
    if (nearestPoint.y > m_max.y)
    {
        nearestPoint.y = m_max.y;
    }

    return nearestPoint;
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
    return m_min + uv * GetDimensions();
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& position) const
{
    Vec2 uv = Vec2(RangeMap(position.x, m_min.x, m_max.x, 0.f, 1.f), RangeMap(position.y, m_min.y, m_max.y, 0.f, 1.f));
    return uv;
}

float AABB2::GetAspect() const
{
    Vec2 dim = GetDimensions();
    return dim.x / dim.y;
}

float AABB2::GetWidth() const
{
    return GetDimensions().x;
}

float AABB2::GetHeight() const
{
    return GetDimensions().y;
}

void AABB2::Pad(float horizontalPadding, float verticalPadding)
{
    m_min.x -= horizontalPadding;
    m_min.y -= verticalPadding;
    m_max.x += horizontalPadding;
    m_max.y += verticalPadding;
}

void AABB2::Translate(Vec2 const& translation)
{
    m_max += translation;
    m_min += translation;
}

AABB2 AABB2::GetTranslated(Vec2 const& translation) const
{
    AABB2 copy = *this;
	copy.Translate(translation);
	return copy;
}

Vec2 AABB2::GetTopLeft() const
{
    Vec2 result;
    result.x = m_min.x;
    result.y = m_max.y;
    return result;
}

Vec2 AABB2::GetTopRight() const
{
    return m_max;
}

Vec2 AABB2::GetBottomLeft() const
{
    return m_min;
}

Vec2 AABB2::GetBottomRight() const
{
    Vec2 result;
    result.x = m_max.x;
    result.y = m_min.y;
    return result;
}

void AABB2::SetTopLeft(Vec2 const& topLeftCoords)
{
    m_min.x = topLeftCoords.x;
    m_max.y = topLeftCoords.y;
}

void AABB2::SetTopRight(Vec2 const& topRightCoords)
{
    m_max = topRightCoords;
}

void AABB2::SetBottomLeft(Vec2 const& bottomLeftCoords)
{
    m_min = bottomLeftCoords;
}

void AABB2::SetBottomRight(Vec2 const& bottomRightCoords)
{
    m_max.x = bottomRightCoords.x;
    m_min.y = bottomRightCoords.y;
}

void AABB2::SetCenter(Vec2 const& center)
{
    Vec2 translation = center - GetCenter();
    Translate(translation);
}

void AABB2::SetDimensions(Vec2 const& dimensions)
{
    Vec2 center = GetCenter();
    m_max = dimensions;
    m_min = Vec2(0, 0);
    SetCenter(center);
}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{
    if (point.x < m_min.x)
    {
        m_min.x = point.x;
    }
    if (point.x > m_max.x)
    {
        m_max.x = point.x;
    }
    if (point.y < m_min.y)
    {
        m_min.y = point.y;
    }
    if (point.y > m_max.y)
    {
        m_max.y = point.y;
    }
}
