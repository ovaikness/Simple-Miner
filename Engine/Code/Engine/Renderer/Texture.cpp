#include "Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

Texture::Texture()
{
}

Texture::~Texture()
{
	SafeRelease(m_texture);
	SafeRelease(m_shaderResourceView);
	SafeRelease(m_renderTargetView);
}
