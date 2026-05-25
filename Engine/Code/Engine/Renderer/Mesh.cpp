#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

CPUMesh::CPUMesh(std::filesystem::path path, Mat44 const& transform)
{
	Create(path, transform);
}

CPUMesh::~CPUMesh()
{

}

void CPUMesh::Create(std::filesystem::path path, Mat44 const& transform)
{
	bool hasNormals = false;
	bool hasUVs = false;

	if (!LoadObj(path, m_vertices, m_indices, hasNormals, hasUVs, transform))
	{
		ERROR_AND_DIE("Failed to load obj file");
	}

	bool calculateNormals  = !hasNormals;
	bool calculateTangents = hasUVs;

	CalculateTangentSpaceBasisVectors(m_vertices, m_indices, calculateNormals, calculateTangents);
}

void CPUMesh::Create(std::string_view data, Mat44 const& transform)
{
	bool hasNormals = false;
	bool hasUVs = false;

	if (!LoadObj(data, m_vertices, m_indices, hasNormals, hasUVs, transform))
	{
		ERROR_AND_DIE("Failed to load obj file");
	}

	bool calculateNormals = !hasNormals;
	bool calculateTangents = hasUVs;

	CalculateTangentSpaceBasisVectors(m_vertices, m_indices, calculateNormals, calculateTangents);
}

void CPUMesh::DebugRenderNormals(Renderer* renderer, float length) const
{
	std::vector<Vertex_PCU> verts;
	for (size_t i = 0; i < m_indices.size(); i++)
	{
		Vertex_PCUTBN const* vert = &m_vertices[m_indices[i]];
		verts.push_back(Vertex_PCU(vert->m_position, Rgba8::BLUE, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vert->m_position + vert->m_normal * length, Rgba8::BLUE, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vert->m_position, Rgba8::RED, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vert->m_position + vert->m_tangent * length, Rgba8::RED, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vert->m_position, Rgba8::GREEN, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vert->m_position + vert->m_bitangent * length, Rgba8::GREEN, Vec2::ZERO));
	}
	renderer->SetPrimitiveTopology(PrimitiveMode::LINES);
	renderer->BindShader(nullptr);
	renderer->BindTexture(nullptr);
	renderer->DrawVertexArray(verts);
	renderer->SetPrimitiveTopology(PrimitiveMode::TRIANGLES);
}

GPUMesh::GPUMesh(Renderer* renderer, CPUMesh const& cpuMesh)
{
	Create(renderer, cpuMesh);
}

GPUMesh::~GPUMesh()
{
	delete m_vbo;
	delete m_ibo;
}

void GPUMesh::Create(Renderer* renderer, CPUMesh const& cpuMesh)
{
	m_vbo = renderer->CreateVertexBufferFromVertexPCUTBNArray(cpuMesh.m_vertices);
	m_ibo = renderer->CreateIndexBuffer(cpuMesh.m_indices);
}

void GPUMesh::Render(Renderer* renderer) const
{
	renderer->BindVertexBuffer(m_vbo);
	renderer->BindIndexBuffer(m_ibo);
	renderer->DrawIndexedVertexBuffer(m_vbo,m_ibo,m_ibo->GetSize() / sizeof(unsigned int));
}