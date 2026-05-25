#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Camera.hpp"

Camera::Camera(AABB2 const& viewport)
	: m_viewport(viewport)
{
}

Vec2 Camera::GetCenter() const
{
	return m_view.GetCenter();
}

void Camera::SetCenter(Vec2 const& center)
{
	m_view.SetCenter(center);
}

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight)
{
	m_mode = ORTHOGRAPHIC;
	m_view = AABB2(bottomLeft, topRight);
	m_orthographicFar = 1.f;
	m_orthographicNear = 0.f;
}

void Camera::SetViewport(AABB2 const& viewport)
{
	m_viewport = viewport;
}

void Camera::SetPerspectiveView(float aspect, float fov, float _near, float _far)
{
	m_mode = PERSPECTIVE;
	m_perspectiveAspect = aspect;
	m_perspectiveFov = fov;
	m_perspectiveNear = _near;
	m_perspectiveFar = _far;
}

void Camera::SetViewportMode(ViewportMode mode)
{
	m_viewportMode = mode;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_view.m_min;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_view.m_max;
}

Vec3 Camera::GetForward() const
{
	Mat44 proj = GetViewMatrix();
	proj.Orthonormalize_IFwd_JLeft_KUp();

	Vec3 forward = Vec3(proj.m_Values[Mat44::Ix], proj.m_Values[Mat44::Jx], proj.m_Values[Mat44::Kx]);
	return forward;
}

Vec3 Camera::GetLeft() const
{
	Mat44 proj = GetViewMatrix();
	proj.Orthonormalize_IFwd_JLeft_KUp();

	Vec3 left = -Vec3(proj.m_Values[Mat44::Iy], proj.m_Values[Mat44::Jy], proj.m_Values[Mat44::Ky]);
	return left;
}

Vec3 Camera::GetUp() const
{
	Mat44 proj = GetViewMatrix();
	proj.Orthonormalize_IFwd_JLeft_KUp();

	Vec3 up = Vec3(proj.m_Values[Mat44::Iz], proj.m_Values[Mat44::Jz], proj.m_Values[Mat44::Kz]);
	return up;
}

float Camera::GetAspectInWindow(Window const& window) const
{
	IntVec2 dim = window.GetClientDimensions();
	Vec2 dimF = Vec2(static_cast<float>(dim.x), static_cast<float>(dim.y));
	Vec2 scaledDim = m_viewport.GetDimensions();
	scaledDim.x *= dimF.x;
	scaledDim.y *= dimF.y;

	return scaledDim.x / scaledDim.y;
}

void Camera::Translate2D(Vec2 const& translation)
{
	m_view.Translate(translation);
}

void Camera::UpdateTranformRelative(Vec3 const& translation, EulerAngles const& deltaOrientation)
{
	Vec3 translationRel = Vec3(0,0,0);
	Mat44 proj = GetViewMatrix();
	proj.Orthonormalize_IFwd_JLeft_KUp();

	Vec3 forward =  Vec3(proj.m_Values[Mat44::Ix], proj.m_Values[Mat44::Jx], proj.m_Values[Mat44::Kx]);
	Vec3 right   = -Vec3(proj.m_Values[Mat44::Iy], proj.m_Values[Mat44::Jy], proj.m_Values[Mat44::Ky]);
	Vec3 up      =  Vec3(proj.m_Values[Mat44::Iz], proj.m_Values[Mat44::Jz], proj.m_Values[Mat44::Kz]);

	translationRel -= forward * translation.x;
	translationRel -= right * translation.y;
	translationRel -= up * translation.z;

	m_position -= translationRel;

	m_orientation.m_yaw -= deltaOrientation.m_yaw;
	m_orientation.m_pitch -= deltaOrientation.m_pitch;
	m_orientation.m_roll -= deltaOrientation.m_roll;
}

EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}

void Camera::SetTransform(Vec3 const& position, EulerAngles const& orientation)
{
	m_position = position;
	m_orientation = EulerAngles(orientation.m_yaw,orientation.m_pitch,orientation.m_roll);
}

void Camera::SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
	m_renderIBasis = iBasis;
	m_renderJBasis = jBasis;
	m_renderKBasis = kBasis;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

Mat44 Camera::GetRenderMatrix() const
{
	Mat44 result;
	result.SetIBasis3D(m_renderIBasis);
	result.SetJBasis3D(m_renderJBasis);
	result.SetKBasis3D(m_renderKBasis);
	return result;
}

Mat44 Camera::GetOrthographicMatrix() const
{
	return Mat44::CreateOrthoProjection(
		m_view.m_min.x 
		, m_view.m_max.x
		, m_view.m_min.y
		, m_view.m_max.y
		,0.f
		,1.f
	);
}

Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::CreatePerspectiveProjection(
		m_perspectiveFov,
		m_perspectiveAspect,
		m_perspectiveNear,
		m_perspectiveFar
	);
}

Mat44 Camera::GetProjectionMatrix() const
{
	switch (m_mode)
	{
	case ORTHOGRAPHIC: return GetOrthographicMatrix().Append(GetRenderMatrix());
	case PERSPECTIVE: return GetPerspectiveMatrix().Append(GetRenderMatrix());
	}

	ERROR_AND_DIE("Camera mode not supported!");
}

Mat44 Camera::GetModelMatrix() const
{
	Mat44 result = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	result.SetTranslation3D(m_position);
	return result;
}

Mat44 Camera::GetViewMatrix() const {
	Mat44 result = Mat44::CreateTranslation3D(m_position);
	EulerAngles invOrientation = EulerAngles(m_orientation.m_yaw, m_orientation.m_pitch, m_orientation.m_roll);
	Mat44 rotation = invOrientation.GetAsMatrix_IFwd_JLeft_KUp();
	result.Append(rotation);

	return result.GetOrthonormalInverse();
}

AABB2 Camera::GetView() const
{
	return m_view;
}

AABB2 Camera::GetViewport(Window const& window) const
{
	(void)window;
	return m_viewport;
}
