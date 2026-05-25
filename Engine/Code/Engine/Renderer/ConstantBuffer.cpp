#include "ConstantBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <d3d11.h>
#include "Engine/Renderer/Renderer.hpp"

ConstantBuffer::ConstantBuffer(size_t size)
	: m_size(size)
{
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(m_buffer);
}
