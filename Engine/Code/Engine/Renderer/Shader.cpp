#include "Shader.hpp"
#include "Renderer.hpp"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDERER
#endif

#if defined(ENGINE_DEBUG_RENDERER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

Shader::Shader(ShaderConfig const& config)
	: m_config(config)
{
}

Shader::~Shader()
{
	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeRelease(m_inputLayout);
}

std::string const& Shader::GetName() const
{
	return m_config.m_name;
}

