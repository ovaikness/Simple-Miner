#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"

IndexBuffer::IndexBuffer(unsigned int size)
	: m_size(size)
{
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(m_buffer);
}

unsigned int IndexBuffer::GetSize() const
{
	return m_size;
}
