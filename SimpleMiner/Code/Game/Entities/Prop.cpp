#include "Prop.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

Prop::Prop(Game* game) : Entity(game)
{
}

void Prop::Update(float deltaSeconds)
{
	EulerAngles deltaRotation = EulerAngles(
		m_angularVelocity.m_yaw * deltaSeconds,
		m_angularVelocity.m_pitch * deltaSeconds,
		m_angularVelocity.m_roll * deltaSeconds
	);

	Mat44 rotation = deltaRotation.GetAsMatrix_IFwd_JLeft_KUp();

	for (Vertex_PCU& vertex : m_vertices)
	{
		vertex.m_position = rotation.TransformPosition3D(vertex.m_position);
	}
}

void Prop::Render()
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.Append(Mat44::CreateTranslation3D(m_position));

	g_theRenderer->SetModelConstants(model,m_color);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray(m_vertices);
}

