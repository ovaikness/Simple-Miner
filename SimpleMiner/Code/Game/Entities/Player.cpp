#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

#include "Game/Game.hpp"
#include "Game/Entities/Player.hpp"

#include <sstream>

Player::Player(Game* game) 
	: Entity(game)
{
	m_position = Vec3(0.f, 0.f, 100.f);
	m_camera.SetPerspectiveView(g_theWindow->GetAspect(), 60.f, 0.1f, 300.f);
	Vec3 renderIBasis = Vec3(0.f, 0.f, 1.f);
	Vec3 renderJBasis = Vec3(-1.f, 0.f, 0.f);
	Vec3 renderKBasis = Vec3(0.f, 1.f, 0.f);
	m_camera.SetRenderBasis(renderIBasis, renderJBasis, renderKBasis);

	m_game->SetActiveCamera(m_camera);
}

void Player::HandleInput(float deltaSeconds)
{
	Vec2 translationAxis = Vec2::ZERO;
	Vec2 orientationAxis = Vec2::ZERO;
	float velocity = 4.f;
	float degreesPerSecond = 180.f;
	InputSystem& in = *g_theInput;
	Vec3 translation = Vec3(0.f, 0.f, 0.f);
	EulerAngles& orientation = m_orientation;

	if (in.IsKeyDown(KEYCODE_SPACE))
	{
		velocity *= 10.f;
	}

	if (in.IsKeyDown('W'))
	{
		translation.x += deltaSeconds * velocity;
	}
	if (in.IsKeyDown('S'))
	{
		translation.x -= deltaSeconds * velocity;
	}
	if (in.IsKeyDown('A'))
	{
		translation.y -= deltaSeconds * velocity;
	}
	if (in.IsKeyDown('D'))
	{
		translation.y += deltaSeconds * velocity;
	}

	if (in.IsKeyDown(KEYCODE_UP))
	{
		orientation.m_pitch += deltaSeconds * degreesPerSecond;
	}
	if (in.IsKeyDown(KEYCODE_DOWN))
	{
		orientation.m_pitch -= deltaSeconds * degreesPerSecond;
	}
	if (in.IsKeyDown(KEYCODE_LEFT))
	{
		orientation.m_yaw -= deltaSeconds * degreesPerSecond;
	}
	if (in.IsKeyDown(KEYCODE_RIGHT))
	{
		orientation.m_yaw += deltaSeconds * degreesPerSecond;
	}

	IntVec2 cursorDelta = in.GetCursorClientDelta();
	orientation.m_yaw   -= cursorDelta.x / 10.f;
	orientation.m_pitch += cursorDelta.y / 10.f;

	XboxController& controller = *in.GetController(0);
	if (controller.IsConnected())
	{
		if (controller.GetButton(GAMEPAD_BUTTON_START).m_wasPressedLastFrame)
		{
			m_position = Vec3(0.f, 0.f, 0.f);
		}
		if (controller.GetButton(GAMEPAD_BUTTON_A).m_isPressed)
		{
			velocity = 10.f;
		}

		Vec2 pos = controller.GetRightStick().GetPosition();
		orientation.m_yaw += pos.x / 10.f;
		orientation.m_pitch += pos.y / 10.f;

		Vec2 axis = Vec2::MakeFromPolarDegrees(controller.GetLeftStick().GetOrientationDegrees(),
			controller.GetLeftStick().GetMagnitude());
		axis.ClampLength(1.f);
		if (axis.GetLength() > 0.f)
		{
			Vec2 translation2d = axis * velocity * deltaSeconds;
			translation.x = translation2d.y;
			translation.y = translation2d.x;
		}
		float levAxis = controller.GetButton(GAMEPAD_BUMPER_LEFT).m_isPressed ? -1.f : 0.f +
			controller.GetButton(GAMEPAD_BUMPER_RIGHT).m_isPressed ? 1.f : 0.f;
		if (levAxis != 0.f)
		{
			translation.z = levAxis * velocity * deltaSeconds;
		}
		
	}

	if (in.WasKeyJustPressed('H'))
	{
		m_position = Vec3(0.f, 0.f, 0.f);
	}

	Vec3 lift = Vec3(0.f, 0.f, 0.f);
	if (in.IsKeyDown('Q'))
	{
		lift += Vec3(0.f, 0.f, velocity) * deltaSeconds;
	}
	if (in.IsKeyDown('E'))
	{
		lift += Vec3(0.f, 0.f, -velocity) * deltaSeconds;
	}

	Vec3 trans = 
		  translation.x * Vec3(m_camera.GetForward().x,m_camera.GetForward().y,0).GetNormalized()
		+ translation.y * Vec3(m_camera.GetLeft().x,m_camera.GetLeft().y,0).GetNormalized()
		+ translation.z * m_camera.GetUp();
	trans.z = 0;

	m_position += trans;

	m_position += lift;

	m_orientation.m_pitch = GetClamped(m_orientation.m_pitch, -85.f, 85.f);
	m_orientation.m_roll  = GetClamped(m_orientation.m_roll,  -45.f, 45.f);

	m_camera.SetTransform(m_position, m_orientation);
}

void Player::Update(float deltaSeconds)
{
	std::stringstream ssp;
	std::stringstream sst;
	std::stringstream ssf;
	std::stringstream ssts;
	ssp << "Position : " << m_position.x << "," << m_position.y << "," << m_position.z;
	sst << "Time : " << Clock::GetSystemClock().GetTotalSeconds();
	float delta = Clock::GetSystemClock().GetDeltaSeconds();
	ssf << "FPS : " << static_cast<int>(delta != 0.f ? 1.f / Clock::GetSystemClock().GetDeltaSeconds() : 0.f);
	ssts <<	"Time Scale : " << Clock::GetSystemClock().GetTimeScale();
	DebugAddScreenText(ssp.str(), Vec2(135.f, 25.f + 73.f), 1.5f, Vec2(0.f, 0.f), 0.f, Rgba8::WHITE, Rgba8::WHITE);
	DebugAddScreenText(sst.str(), Vec2(172.f, 25.f + 71.f), 1.5f, Vec2(0.f, 0.f), 0.f, Rgba8::WHITE, Rgba8::WHITE);
	DebugAddScreenText(ssf.str(), Vec2(172.f, 25.f + 69.f), 1.5f, Vec2(0.f, 0.f), 0.f, Rgba8::WHITE, Rgba8::WHITE);
	DebugAddScreenText(ssts.str(), Vec2(172.f, 25.f + 67.f), 1.5f, Vec2(0.f, 0.f), 0.f, Rgba8::WHITE, Rgba8::WHITE);
	DebugRenderSystemSetTarget(m_camera.GetModelMatrix());
	HandleInput(deltaSeconds);
}

void Player::Render()
{
}
