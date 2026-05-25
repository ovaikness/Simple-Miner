#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include <cmath>

Entity::~Entity()
{
}

Entity::Entity(Game* game)
	: m_game(game)
	, m_position(Vec3(0.f,0.f,0.f))
	, m_orientation(EulerAngles(0.f,0.f,0.f))
	, m_velocity(Vec3(0.f,0.f,0.f))
	, m_angularVelocity(EulerAngles(0.f,0.f,0.f))
{

}