#pragma once
#include "Engine/Renderer/Camera.hpp"

#include "Game/Entities/Entity.hpp"

class Player : public Entity
{
public:
	Camera m_camera;
public:
	Player(Game* game);

	void HandleInput(float deltaSeconds);
	void Update(float deltaSeconds) override;
	void Render() override;
};