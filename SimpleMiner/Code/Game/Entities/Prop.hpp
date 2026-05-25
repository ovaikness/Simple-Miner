#pragma once
#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

class Prop : public Entity
{
public:
	Prop(Game* game);

	void Update(float deltaSeconds) override;
	void Render() override;

public:
	std::vector<Vertex_PCU> m_vertices;
	Rgba8				    m_color = Rgba8(255,255,255,255);
	Texture*				m_texture = nullptr;
};