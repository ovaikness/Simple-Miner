#pragma once

#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Vec3.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

#include <filesystem>
#include <vector>

class CPUMesh
{
public:
	CPUMesh() = default;
	CPUMesh(std::filesystem::path path, Mat44 const& transform);

	virtual ~CPUMesh();

	void Create(std::filesystem::path path, Mat44 const& transform);
	void Create(std::string_view data, Mat44 const& transform);

	void DebugRenderNormals(Renderer* renderer, float length = 0.05f) const;

	std::vector<Vertex_PCUTBN> m_vertices;
	std::vector<unsigned int> m_indices;
};

class GPUMesh
{
public:
	GPUMesh() = default;
	GPUMesh(Renderer* renderer, CPUMesh const& cpuMesh);

	virtual ~GPUMesh();

	void Create(Renderer* renderer, CPUMesh const& cpuMesh);
	void Render(Renderer* renderer) const;

	size_t m_indexCount = 0;
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
};