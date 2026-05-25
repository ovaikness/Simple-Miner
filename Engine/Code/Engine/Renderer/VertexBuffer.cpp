#include "VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

VertexBuffer::VertexBuffer(size_t size)
	: m_size(size)
{

}

VertexBuffer::~VertexBuffer()
{
	SafeRelease(m_buffer);
}
