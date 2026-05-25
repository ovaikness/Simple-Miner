#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"

class Window;

enum CameraMode
{
	ORTHOGRAPHIC,
	PERSPECTIVE,
	CAMERA_MODE_COUNT
};

enum ViewportMode
{
	STRETCH,
	STRETCH_KEEP_ASPECT,
	CROP_KEEP_ASPECT,
};

class Camera
{
public:
	Vec3 m_position = Vec3(0.f, 0.f, 0.f);
	EulerAngles m_orientation = {};
public:
	Camera(AABB2 const& viewport = AABB2(0.f,0.f,1.f,1.f));

	Vec2 GetCenter() const;
	void SetCenter(Vec2 const& center);
	void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight);
	void SetViewport(AABB2 const& viewport);

	void SetPerspectiveView(float aspect, float fov, float _near, float _far);
	void SetViewportMode(ViewportMode mode);

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	Vec3 GetForward() const;
	Vec3 GetLeft() const;
	Vec3 GetUp() const;

	AABB2 GetView() const;
	AABB2 GetViewport(Window const& window) const;

	float GetAspectInWindow(Window const& window) const;
	void Translate2D(Vec2 const& translation);

	void UpdateTranformRelative(Vec3 const& translation, EulerAngles const& deltaOrientation);
	void SetTransform(Vec3 const& position, EulerAngles const& orientation);
	void SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis);

	EulerAngles GetOrientation() const;

	Vec3  GetPosition() const;
	Mat44 GetRenderMatrix() const;
	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;
	Mat44 GetModelMatrix() const;
	Mat44 GetViewMatrix() const;
private:
	CameraMode m_mode = ORTHOGRAPHIC;
	ViewportMode m_viewportMode = STRETCH;

	Vec3 m_renderIBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 m_renderJBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 m_renderKBasis = Vec3(0.f, 0.f, 1.f);

	AABB2 m_view = {};
	AABB2 m_viewport = {};

	float m_orthographicNear = 0.f;
	float m_orthographicFar = 0.f;

	float m_perspectiveAspect = 0.f;
	float m_perspectiveFov = 0.f;
	float m_perspectiveNear = 0.f;
	float m_perspectiveFar = 0.f;
};