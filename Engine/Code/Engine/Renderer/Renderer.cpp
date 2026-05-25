#include "Renderer.hpp"
#include "Camera.hpp"
#include <Windows.h>
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/SquirrelFixedFont.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/Vertex_Font.hpp"

#include <string.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#if defined(ENGINE_DEBUG_RENDERER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Window/Window.hpp"

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Mat44.hpp"
#ifdef OPAQUE
#undef OPAQUE
#endif
struct LightConstants
{
	Vec3 sunDirection;
	float sunIntensity;
	float ambientIntensity;
	Vec3 worldEyePos;

	float minFalloff;
	float maxFalloff;
	float specularPower;
	float specularFactor;

	LightDebug debug;
};

struct CameraConstants
{
	Mat44 ProjectionMatrix;
	Mat44 ViewMatrix;
};

struct ModelConstants
{
	Mat44 ModelMatrix;
	Vec4  ModelColor;
};

struct GameConstants
{
	float Time;
	float padding[3];
};

struct MinerConstants
{
	Vec4 CameraPos;
	Vec4 InteriorLight = Vec4(255, 255, 255, 255);
	Vec4 ExteriorLight = Vec4(255, 230, 204, 255);
	Vec4 SkyColor = Vec4(0, 0, 0, 255);
	float FogFar = 0;
	float FogNear = 0;
	float Padding1;
	float Padding2;
};

struct BlurSample
{
	Vec2 Offset;
	float Weight;
	float Padding;
};

static int const k_blurMaxSamples = 16;

struct BlurConstants
{
	Vec2 TexelSize;
	float LerpT;
	int NumSamples;
	BlurSample Samples[k_blurMaxSamples];
};

static int const k_cameraConstantsSlot = 2;
static int const k_modelConstantsSlot = 3;
static int const k_lightConstantsSlot = 4;
static int const k_minerConstantsSlot = 5;
static int const k_blurConstantsSlot = 6;
static int const k_gameConstantsSlot = 7;

Renderer::Renderer()
{

}

Renderer::Renderer(RenderConfig const& config)
	: m_config(config)
{

}

void Renderer::InitDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	HRESULT hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStates[(size_t)DepthMode::DISABLED]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize depth states!");
	}

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStates[(size_t)DepthMode::READ_ONLY_ALWAYS]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize depth states!");
	}
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStates[(size_t)DepthMode::READ_WRITE_LESS_EQUAL]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize depth states!");
	}
}

void Renderer::InitRasterizerStates()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(size_t)RasterizerMode::SOLID_CULL_NONE]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize rasterizer states!");
	}

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(size_t)RasterizerMode::SOLID_CULL_BACK]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize rasterizer states!");
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(size_t)RasterizerMode::WIREFRAME_CULL_NONE]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize rasterizer states!");
	}


	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(size_t)RasterizerMode::WIREFRAME_CULL_BACK]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Failed to initialize rasterizer states!");
	}
}

void Renderer::InitSamplerStates()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr;
	hr = m_device->CreateSamplerState(&samplerDesc,
		&m_samplerStates[(size_t)SamplerMode::POINT_CLAMP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Create sampler state for SamplerMode::POINT_CLAMP failed!");
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = m_device->CreateSamplerState(&samplerDesc,
		&m_samplerStates[(size_t)SamplerMode::BILINEAR_WRAP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Create sampler state for SamplerMode::BILINEAR_WRAP failed!");
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = m_device->CreateSamplerState(&samplerDesc,
				&m_samplerStates[(size_t)SamplerMode::BILINEAR_CLAMP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Create sampler state for SamplerMode::BILINEAR_CLAMP failed!");
	}
}

void Renderer::InitBlendStates()
{
	D3D11_BLEND_DESC blendDesc = { };
	auto& rt = blendDesc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D11_BLEND_ONE;
	rt.DestBlend = D3D11_BLEND_ZERO;
	rt.BlendOp = D3D11_BLEND_OP_ADD;
	rt.SrcBlendAlpha = rt.SrcBlend;
	rt.DestBlendAlpha = rt.DestBlend;
	rt.BlendOpAlpha = rt.BlendOp;
	rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(size_t)(BlendMode::OPAQUE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed!");
	}

	rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rt.SrcBlendAlpha = rt.SrcBlend;
	rt.DestBlendAlpha = rt.DestBlend;

	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(size_t)(BlendMode::ALPHA)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ALPHA failed!");
	}

	rt.SrcBlend = D3D11_BLEND_ONE;
	rt.DestBlend = D3D11_BLEND_ONE;
	rt.SrcBlendAlpha = rt.SrcBlend;
	rt.DestBlendAlpha = rt.DestBlend;

	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(size_t)(BlendMode::ADDITIVE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ADDITIVE failed!");
	}
}

void Renderer::Startup()
{
	//Create debug module
#if defined(ENGINE_DEBUG_RENDERER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(__uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module.");
	}
#endif

	//Renderer Startup
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDERER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	auto window = m_config.m_window;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain.");
	}

	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create render target view for swap chain buffer.");
	}

	backBuffer->Release();

	//From source
	m_defaultShader = CreateShader("Default", defaultShaderSource);
	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex_PCU));
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstants));
	m_modelCBO = CreateConstantBuffer(sizeof(ModelConstants));
	m_lightCBO = CreateConstantBuffer(sizeof(LightConstants));
	m_minerCBO = CreateConstantBuffer(sizeof(MinerConstants));
	m_blurCBO = CreateConstantBuffer(sizeof(BlurConstants));
	m_gameCBO = CreateConstantBuffer(sizeof(GameConstants));

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_config.m_window->GetClientDimensions().x;
	textureDesc.Height = m_config.m_window->GetClientDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.SampleDesc.Count = 1;

	hr = m_device->CreateTexture2D(&textureDesc, nullptr, &m_depthStencilTexture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create texture for depth stencil!");
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth stencil view!");
	}

	InitDepthStencilStates();
	InitRasterizerStates();
	InitBlendStates();
	InitSamplerStates();

	uint8_t* defaultData = new uint8_t[4 * sizeof(Rgba8)]{
		255,255,255,255, 255,255,255,255,
		255,255,255,255, 255,255,255,255
	};
	m_defaultTexture = CreateTextureFromData("Default", IntVec2(2, 2), 4, defaultData);
	std::vector<uint8_t> defaultFontData;
	for (int yy = 255; yy >= 0; --yy)
	{
		for (int xx = 0; xx <= 255; ++xx)
		{
			if (defaultFontImageData[xx + yy * 256] != 0)
			{
				defaultFontData.push_back(255);
				defaultFontData.push_back(255);
				defaultFontData.push_back(255);
				defaultFontData.push_back(255);
			}
			else
			{
				defaultFontData.push_back(0);
				defaultFontData.push_back(0);
				defaultFontData.push_back(0);
				defaultFontData.push_back(0);
			}
		}
	}
	Texture* defaultFontText = CreateTextureFromData("DefaultFont", IntVec2(256, 256), 4, defaultFontData.data());
	m_loadedBitmapFonts["DefaultFont"] = new BitmapFont("DefaultFont", *defaultFontText);
	BindTexture(m_defaultTexture);
	BindShader(m_defaultShader);

	std::vector<Vertex_PCU> quadVBOVerts
	{
		// First triangle
		Vertex_PCU(Vec3(1.f,  1.f, 0.f), Rgba8::WHITE, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(1.f, -1.f, 0.f), Rgba8::WHITE, Vec2(1.f, 1.f)),
		Vertex_PCU(Vec3(-1.f, -1.f, 0.f), Rgba8::WHITE, Vec2(0.f, 1.f)),

		// Second triangle
		Vertex_PCU(Vec3(-1.f,  1.f, 0.f), Rgba8::WHITE, Vec2(0.f, 0.f)),
		Vertex_PCU(Vec3(1.f,  1.f, 0.f), Rgba8::WHITE, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, -1.f, 0.f), Rgba8::WHITE, Vec2(0.f, 1.f)),
	};

	m_fullscreenVBO = CreateVertexBufferFromVertexPCUArray(quadVBOVerts);

	CreateEmissiveRenderTargets();
}

void Renderer::SetRenderTarget(Texture* target)
{
	if (target)
	{
		m_deviceContext->OMSetRenderTargets(1, &target->m_renderTargetView, NULL);
	}
	else
	{
		Texture* otherTarget = m_emissiveRenderTargets[0];
		std::vector<ID3D11RenderTargetView*> renderTargets
		{
					m_renderTargetView,
			otherTarget->m_renderTargetView
		};

		m_deviceContext->OMSetRenderTargets(2, renderTargets.data(), m_depthStencilView);
	}
}

void Renderer::BeginFrame()
{
	Texture* target = m_emissiveRenderTargets[0];
	std::vector<ID3D11RenderTargetView*> renderTargets 
	{
		m_renderTargetView,
		target->m_renderTargetView
	};

	m_deviceContext->OMSetRenderTargets(2, renderTargets.data(), m_depthStencilView);

	m_sunIntensity = GetClamped(m_sunIntensity, 0.f, 1.f);
	m_ambientIntensity = GetClamped(m_ambientIntensity, 0.f, 1.f);

	SetLightConstants(
		m_sunDirection.GetNormalized(), 
		m_sunIntensity, 
		m_ambientIntensity,
		m_worldEyePos,
		m_minFalloff,
		m_maxFalloff,
		m_specularPower,
		m_specularFactor,
		m_lightDebug
	);

	float time = (float)Clock::GetSystemClock().GetTotalSeconds();

	SetGameConstants(
		time
	);
}

void Renderer::EndFrame()
{
	RenderEmissive();
	HRESULT hr;

	hr = m_swapChain->Present(0, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}

	for (Texture* tex : m_emissiveRenderTargets)
	{
		ClearRenderTargetTexture(tex);
	}

	for (Texture* tex : m_emissiveRenderTargetsUpscale)
	{
		ClearRenderTargetTexture(tex);
	}
}

void Renderer::Shutdown()
{
	delete m_minerCBO;
	delete m_modelCBO;
	delete m_cameraCBO;
	delete m_immediateVBO;
	delete m_lightCBO;
	delete m_blurCBO;
	delete m_gameCBO;

	delete m_fullscreenVBO;

	for (int shaderIndex = 0; shaderIndex < m_loadedShaders.size(); ++shaderIndex)
	{
		delete m_loadedShaders[shaderIndex];
		m_loadedShaders[shaderIndex] = nullptr;
	}

	//m_blendState is a pointer to an element in the array and therefore does not need
	//to be released
	for (int blendStateIndex = 0; blendStateIndex < (int)BlendMode::COUNT; ++blendStateIndex)
	{
		SafeRelease(m_blendStates[blendStateIndex]);
	}

	for (int samplerModeIndex = 0; samplerModeIndex < (int)SamplerMode::COUNT; ++samplerModeIndex)
	{
		SafeRelease(m_samplerStates[samplerModeIndex]);
	}

	for (int rasterizerModeIndex = 0; rasterizerModeIndex < (int)RasterizerMode::COUNT; ++rasterizerModeIndex)
	{
		SafeRelease(m_rasterizerStates[rasterizerModeIndex]);
	}

	for (int depthStencilStateIndex = 0; depthStencilStateIndex < (int)DepthMode::COUNT; ++depthStencilStateIndex)
	{
		SafeRelease(m_depthStates[depthStencilStateIndex]);
	}

	for (std::pair<std::string const, Texture*>& texturePair : m_loadedTextures)
	{
		delete texturePair.second;
	}

	SafeRelease(m_depthStencilView);
	SafeRelease(m_depthStencilTexture);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_swapChain);
	SafeRelease(m_deviceContext);
	SafeRelease(m_device);

	for (std::pair<std::string const, BitmapFont*>& fontPair : m_loadedBitmapFonts)
	{
		delete fontPair.second;
	}
	m_loadedTextures.clear();

#ifdef ENGINE_DEBUG_RENDERER
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);

	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
}

void Renderer::BeginCamera(const Camera& camera)
{
	m_boundCamera = &camera;
	ModelConstants mc;
	mc.ModelMatrix = Mat44::IDENTITY;
	mc.ModelColor = Vec4(1.f,1.f,1.f,1.f);

	CopyCPUToGPU(&mc, sizeof(mc), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);

	CameraConstants cc;
	cc.ProjectionMatrix = camera.GetProjectionMatrix();
	cc.ViewMatrix = camera.GetViewMatrix();
	CopyCPUToGPU(&cc, sizeof(cc), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);

	MinerConstants mcc;
	
	CopyCPUToGPU(&mcc, sizeof(mcc), m_minerCBO);
	BindConstantBuffer(k_minerConstantsSlot, m_minerCBO);

	AABB2 cameraViewport = camera.GetViewport(*m_config.m_window);
	float widthScale = cameraViewport.m_max.x - cameraViewport.m_min.x;
	float heightScale = cameraViewport.m_max.y - cameraViewport.m_min.y;

	IntVec2 windowDim = m_config.m_window->GetClientDimensions();

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = cameraViewport.m_min.x * ((float)windowDim.x);
	viewport.TopLeftY = ((float)windowDim.y) - cameraViewport.m_max.y * ((float)windowDim.y);
	viewport.Width    = ((float)windowDim.x) * widthScale;
	viewport.Height   = ((float)windowDim.y) * heightScale;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::EndCamera(const Camera& camera)
{
	UNUSED(camera);
	m_boundCamera = nullptr;
}

void Renderer::DrawVertexArray(std::vector<Vertex_PCU> const& vertices)
{
	DrawVertexArray(static_cast<int>(vertices.size()), vertices.data());
}

void Renderer::DrawVertexArray(std::vector<Vertex_Font> const& vertices)
{
	DrawVertexArray(static_cast<int>(vertices.size()), vertices.data());
}

void Renderer::SetRasterizerMode(RasterizerMode mode)
{
	m_desiredRasterizerMode = mode;
}

void Renderer::SetSamplerMode(SamplerMode mode)
{
	m_desiredSamplerMode = mode;
}

void Renderer::SetBlendMode(BlendMode mode)
{
	m_desiredBlendMode = mode;
}

void Renderer::SetDepthMode(DepthMode mode)
{
	m_desiredDepthMode = mode;
}

void Renderer::SetLightConstants(
	Vec3 const& sunDirection, 
	float sunIntensity, 
	float ambientIntensity,
	Vec3 worldEyePos, 
	float minFalloff,
	float maxFalloff,
	float specularPower,
	float specularFactor,
	LightDebug const& lightDebug)
{
	LightConstants lc;
	lc.sunDirection = sunDirection.GetNormalized();
	lc.sunIntensity = sunIntensity;
	lc.ambientIntensity = ambientIntensity;
	lc.worldEyePos = worldEyePos;
	lc.minFalloff = minFalloff;
	lc.maxFalloff = maxFalloff;
	lc.specularPower = specularPower;
	lc.specularFactor = specularFactor;
	lc.debug = lightDebug;

	CopyCPUToGPU(&lc, sizeof(lc), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

void Renderer::SetGameConstants(float time)
{
	GameConstants gc;
	gc.Time = time;
	CopyCPUToGPU(&gc, sizeof(gc), m_gameCBO);
	BindConstantBuffer(k_gameConstantsSlot, m_gameCBO);
}

void Renderer::SetModelConstants(Mat44 const& modelMatrix, Rgba8 const& modelColor)
{
	ModelConstants mc;
	mc.ModelMatrix = modelMatrix;
	mc.ModelColor = modelColor.GetAsVec4();

	CopyCPUToGPU(&mc, sizeof(mc), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

void Renderer::SetMinerConstants(Vec3 const& position, Rgba8 const& indoorColor, Rgba8 const& outdoorColor, Rgba8 const& skyColor, float fogNear, float fogFar)
{
	MinerConstants mc;
	mc.CameraPos = Vec4(position.x, position.y, position.z, 1.f);
	mc.ExteriorLight = outdoorColor.GetAsVec4();
	mc.InteriorLight = indoorColor.GetAsVec4();
	mc.FogFar = fogFar;
	mc.FogNear = fogNear;
	mc.SkyColor = skyColor.GetAsVec4();

	CopyCPUToGPU(&mc, sizeof(mc), m_minerCBO);
	BindConstantBuffer(k_minerConstantsSlot, m_minerCBO);
}

void Renderer::SetBlurConstants(Vec2 const& texelSize, float lerpT, std::vector<Vec2> const& offsets, std::vector<float> const& weights)
{
	BlurConstants bc = {};
	bc.LerpT = lerpT;
	bc.NumSamples = (int)offsets.size();
	for (size_t i = 0; i < offsets.size(); ++i)
	{
		bc.Samples[i].Offset = offsets[i];
		bc.Samples[i].Weight = weights[i];
	}
	bc.TexelSize = texelSize;

	CopyCPUToGPU(&bc, sizeof(bc), m_blurCBO);
	BindConstantBuffer(k_blurConstantsSlot, m_blurCBO);
}

void Renderer::DrawVertexArray(int numVertices, Vertex_PCU const* verticeArray)
{
	CopyCPUToGPU(verticeArray, numVertices * sizeof(Vertex_PCU), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertices);
}

void Renderer::DrawVertexArray(int numVertexes, Vertex_Font const* vertexes)
{
	CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_Font), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertexes,0, VertexType::FONT);
}

void Renderer::SetStatesIfChanged()
{
	ID3D11BlendState* desiredBlendState = m_blendStates[(size_t)m_desiredBlendMode];
	if (desiredBlendState != m_blendState)
	{
		m_blendState = desiredBlendState;
		float blendFactor[4] = { 0.f,0.f,0.f,0.f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	ID3D11SamplerState* desiredSamplerState = m_samplerStates[(size_t)m_desiredSamplerMode];
	if (desiredSamplerState != m_samplerState)
	{
		m_samplerState = desiredSamplerState;
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}

	ID3D11RasterizerState* desiredRasterizerState = m_rasterizerStates[(size_t)m_desiredRasterizerMode];
	if (desiredRasterizerState != m_rasterizerState)
	{
		m_rasterizerState = desiredRasterizerState;
		m_deviceContext->RSSetState(desiredRasterizerState);
	}

	ID3D11DepthStencilState* desiredDepthState = m_depthStates[(size_t)m_desiredDepthMode];
	if (desiredDepthState != m_depthState)
	{
		m_depthState = desiredDepthState;
		m_deviceContext->OMSetDepthStencilState(desiredDepthState,0);
	}
}

Camera const* Renderer::GetBoundCamera() const
{
	return m_boundCamera;
}

BitmapFont* Renderer::CreateOrGetBitmapFont(char const* bitmapFontPath)
{
	if (m_loadedBitmapFonts.find(bitmapFontPath) != m_loadedBitmapFonts.end())
	{
		BitmapFont* existingFont = GetBitmapFontForFilename(bitmapFontPath);
		return existingFont;
	}
	BitmapFont* newFont = CreateBitmapFontFromFile(bitmapFontPath);
	return newFont;
}

BitmapFont* Renderer::CreateBitmapFontFromFile(char const* bitmapFontPath)
{
	BitmapFont* theFont = new BitmapFont(bitmapFontPath, *CreateOrGetTextureFromFile((std::string(bitmapFontPath) + ".png").c_str()));
	m_loadedBitmapFonts[bitmapFontPath] = theFont;
	return theFont;

}

BitmapFont* Renderer::GetBitmapFontForFilename(char const* bitmapFontPath)
{
	if (m_loadedBitmapFonts.find(bitmapFontPath) != m_loadedBitmapFonts.end())
	{
		return m_loadedBitmapFonts[bitmapFontPath];
	}
	return nullptr;
}

Texture* Renderer::GetTextureForFileName(char const* imageFilePath)
{
	return m_loadedTextures[imageFilePath];
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	if (m_loadedTextures.find(imageFilePath) != m_loadedTextures.end())
	{
		// See if we already have this texture previously loaded
		Texture* existingTexture = GetTextureForFileName(imageFilePath);
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile(imageFilePath);
	return newTexture;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	IntVec2 dimensions = IntVec2::ZERO;		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, numComponentsRequested);

	// Check if the load was successful
	std::string errorMessage = "Failed to load texture " + std::string(imageFilePath) + "!";
	ASSERT_RECOVERABLE(texelData, errorMessage);

	Texture* newTexture = CreateTextureFromData(imageFilePath, dimensions, bytesPerTexel, texelData);

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free(texelData);
	return newTexture;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
{
	UNUSED(bytesPerTexel);
	// Check if the load was successful
	//GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
	//GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
	//GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = dimensions.x;
	textureDesc.Height = dimensions.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = texelData;
	textureData.SysMemPitch = 4 * dimensions.x;
	
	HRESULT hr;
	hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_RECOVERABLE(Stringf("CreateTextureFromData failed for image \"%s\".",name));
		return nullptr;
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL,
		&newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_RECOVERABLE(Stringf("Failed to create shader resource view for image \"%s\".", name));
		return nullptr;
	}

	m_loadedTextures[name] = newTexture;
	return newTexture;
}

Texture* Renderer::CreateRenderTexture(IntVec2 dimensions, char const* name)
{
	if (m_loadedTextures.find(name) != m_loadedTextures.end())
	{
		return m_loadedTextures[name];
	}
	Texture* newTexture = new Texture();
	newTexture->m_name = name;
	newTexture->m_dimensions = dimensions;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = dimensions.x;
	textureDesc.Height = dimensions.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT hr;
	hr = m_device->CreateTexture2D(&textureDesc, NULL, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_RECOVERABLE(Stringf("CreateTextureFromData failed for image \"%s\".", name));
		return nullptr;
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL,
		&newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_RECOVERABLE(Stringf("Failed to create shader resource view for image \"%s\".", name));
		return nullptr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = m_device->CreateRenderTargetView(newTexture->m_texture, &rtvDesc, &newTexture->m_renderTargetView);
	if (!SUCCEEDED(hr))
	{
		ERROR_RECOVERABLE(Stringf("Failed to create render target view for image \"%s\".", name));
		return nullptr;
	}

	m_loadedTextures[name] = newTexture;
	return newTexture;

}

void Renderer::ClearRenderTargetTexture(Texture* texture)
{
	Rgba8 clearColor = Rgba8(0, 0, 0, 0);
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);

	m_deviceContext->ClearRenderTargetView(texture->m_renderTargetView, colorAsFloats);
}

ConstantBuffer* Renderer::CreateConstantBuffer(size_t const size)
{
	ConstantBuffer* buffer = new ConstantBuffer(size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr;
	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &buffer->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create constant buffer!");
	}
	return buffer;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);
}

void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture(Texture const* texture, int slot /*= 0*/)
{
	if (texture)
	{
		if (m_currentTexture != texture || m_currentSlot != slot)
		{
			m_currentTexture = texture;
			m_currentSlot = slot;
			m_deviceContext->PSSetShaderResources(slot, 1, &m_currentTexture->m_shaderResourceView);
		}
	}
	else
	{
		m_currentTexture = m_defaultTexture;
		m_currentSlot = 0;
		m_deviceContext->PSSetShaderResources(slot, 1, &m_defaultTexture->m_shaderResourceView);
	}
}
Shader* Renderer::GetOrCreateShader(char const* shaderName, VertexType type)
{
	for (Shader* shader : m_loadedShaders)
	{
		if (shader->m_config.m_name == shaderName && shader->m_config.m_type == type)
		{
			return shader;
		}
	}
	Shader* shader = CreateShader(shaderName, type);
	return shader;
}
Shader* Renderer::CreateShader(char const* shaderName, VertexType type)
{
	std::string filePath = "Data/Shaders/" + std::string(shaderName) + ".hlsl";
	std::string shaderSource;
	FileReadToString(shaderSource, filePath);
	return CreateShader(shaderName, shaderSource.c_str(),type);
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource, VertexType type)
{
	ShaderConfig config;
	config.m_name = shaderName;
	config.m_type = type;
	Shader* shader = new Shader(config);

	HRESULT hr;

	std::vector<unsigned char> m_vertexShaderByteCode, m_pixelShaderByteCode;

	CompileShaderToByteCode(m_vertexShaderByteCode, "VertexShader", shaderSource, "VertexMain", "vs_5_0");
	CompileShaderToByteCode(m_pixelShaderByteCode, "PixelShader", shaderSource, "PixelMain", "ps_5_0");

	//Create vertex shader
	hr = m_device->CreateVertexShader(
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		NULL, &shader->m_vertexShader
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex shader."));
	}

	hr = m_device->CreatePixelShader(
		m_pixelShaderByteCode.data(),
		m_pixelShaderByteCode.size(),
		NULL, &shader->m_pixelShader);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader."));
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;

	switch (type)
	{
		case VertexType::PCU:
		{
			inputElementDesc = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			break;
		}
		case VertexType::PCUTBN:
		{
			inputElementDesc = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			break;
		}
		case VertexType::FONT:
		{
			inputElementDesc = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"GLYPH_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXT_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"CHARACTER_INDEX", 0, DXGI_FORMAT_R32_SINT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"EFFECTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			break;
		}
	}

	UINT numElements = (UINT)inputElementDesc.size();
	hr = m_device->CreateInputLayout(
		inputElementDesc.data(), numElements,
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		&shader->m_inputLayout
	);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex layout.");
	}

	m_loadedShaders.push_back(shader);

	return shader;
}

bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	HRESULT hr;
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDERER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags, 0,
		&shaderBlob, &errorBlob
	);

	if (SUCCEEDED(hr))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize()
		);
		SafeRelease(shaderBlob);
		SafeRelease(errorBlob);
		return true;
	}
	else
	{
		if (errorBlob != NULL)
		{
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		SafeRelease(shaderBlob);
		SafeRelease(errorBlob);
		ERROR_AND_DIE(Stringf("Could not compile vertex shader."));
	}
}

void Renderer::BindShader(Shader* shader)
{
	if (shader)
	{
		m_currentShader = shader;
	}
	else
	{
		m_currentShader = m_defaultShader;
	}
}

VertexBuffer* Renderer::CreateVertexBufferFromVertexPCUTBNArray(std::vector<Vertex_PCUTBN> const& verts)
{
	VertexBuffer* buffer = CreateVertexBuffer(verts.size() * sizeof(Vertex_PCUTBN));
	CopyCPUToGPU(verts.data(), verts.size() * sizeof(Vertex_PCUTBN), buffer);
	return buffer;
}

VertexBuffer* Renderer::CreateVertexBufferFromVertexPCUArray(std::vector<Vertex_PCU> const& verts)
{
	// Create the vertex buffer and copy data
	VertexBuffer* buffer = CreateVertexBuffer(verts.size() * sizeof(Vertex_PCU));
	CopyCPUToGPU(verts.data(), verts.size() * sizeof(Vertex_PCU), buffer);

	return buffer;
}
VertexBuffer* Renderer::CreateVertexBuffer(size_t const size)
{
	HRESULT hr;

	VertexBuffer* vbo = new VertexBuffer(size);

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &vbo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex buffer.");
	}

	return vbo;
}

IndexBuffer* Renderer::CreateIndexBuffer(std::vector<unsigned int> const& data)
{
	return CreateIndexBuffer(data.size() * sizeof(unsigned int), data.data());
}

IndexBuffer* Renderer::CreateIndexBuffer(size_t const size, unsigned int const* data)
{
	HRESULT hr;

	IndexBuffer* ibo = new IndexBuffer((unsigned int)size);

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &ibo->m_buffer);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create index buffer.");
	}

#ifdef _DEBUG
	static int index = 0;
	++index;
	std::string indexName = Stringf("Index Buffer %i" , index);
	ibo->m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)indexName.size(), indexName.c_str());
#endif

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);

	return ibo;
}

void Renderer::CopyCPUToGPU(void const* data, size_t size, VertexBuffer* vbo)
{
	if (vbo->m_size < size)
	{
		SafeRelease(vbo->m_buffer);

		HRESULT hr;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = (UINT)size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = m_device->CreateBuffer(&bufferDesc, nullptr, &vbo->m_buffer);
		vbo->m_size = size;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

void Renderer::CopyCPUToGPU(void const* data, size_t size, IndexBuffer* ibo)
{
	if (ibo->m_size < size)
	{
		SafeRelease(ibo->m_buffer);

		HRESULT hr;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = (UINT)size;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = m_device->CreateBuffer(&bufferDesc, nullptr, &ibo->m_buffer);
		ibo->m_size = (unsigned int)size;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);
}

void Renderer::SetPrimitiveTopology(PrimitiveMode mode)
{
	m_primitiveMode = mode;
}

void Renderer::BindVertexBuffer(VertexBuffer* vbo, VertexType type)
{
	UINT stride;
	switch (type)
	{
		case VertexType::PCU :
		{
			stride = sizeof(Vertex_PCU);
			break;
		}
		case VertexType::PCUTBN:
		{
			stride = sizeof(Vertex_PCUTBN);
			break;
		}
		case VertexType::FONT:
		{
			stride = sizeof(Vertex_Font);
			break;
		}
	}
	UINT startOffset = 0;

	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &stride, &startOffset);

	switch (m_primitiveMode)
	{
		case PrimitiveMode::TRIANGLES:
		{
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		}

		case PrimitiveMode::LINES:
		{
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		}
	}
}

void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	UINT startOffset = 0;

	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, startOffset);
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, int vertexOffset, VertexType vertexType)
{
	SetStatesIfChanged();
	m_deviceContext->IASetInputLayout(m_currentShader->m_inputLayout);
	m_deviceContext->VSSetShader(m_currentShader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_currentShader->m_pixelShader, nullptr, 0);
	BindVertexBuffer(vbo, vertexType);
	m_deviceContext->Draw(vertexCount, vertexOffset);
}

void Renderer::DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, int indexCount, int indexOffset, int vertexOffset,VertexType type)
{
	SetStatesIfChanged();
	m_deviceContext->IASetInputLayout(m_currentShader->m_inputLayout);
	m_deviceContext->VSSetShader(m_currentShader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_currentShader->m_pixelShader, nullptr, 0);
	BindVertexBuffer(vbo, type);
	BindIndexBuffer(ibo);
	m_deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
}

void Renderer::CreateEmissiveRenderTargets()
{
	IntVec2 targetDim = m_config.m_window->GetClientDimensions();

	int index = 0;
	while (targetDim.y > 16)
	{
		std::string name = Stringf("EmissiveTarget%i", index);
		std::string nameUpscale = Stringf("EmissiveUpscaleTarget%i", index);
		Texture* emissiveRenderTarget = CreateRenderTexture(targetDim, name.c_str());
		Texture* emissiveRenderTargetUpscale = CreateRenderTexture(targetDim, nameUpscale.c_str());
		m_emissiveRenderTargets.push_back(emissiveRenderTarget);
		m_emissiveRenderTargetsUpscale.push_back(emissiveRenderTargetUpscale);
		targetDim /= 2;
		++index;
	}

}

void Renderer::RenderEmissive()
{
	if (!m_config.m_enableEmissive)
	{
		return;
	}

	SetDepthMode(DepthMode::DISABLED);
	SetSamplerMode(SamplerMode::BILINEAR_CLAMP);

	std::vector<Vec2> const blurDownOffsets = {
		Vec2(-2, 2), Vec2(0 , 2), Vec2(2,2),
		Vec2(-1, 1), Vec2(1,1),
		Vec2(-2,0) , Vec2(0,0), Vec2(2,0),
		Vec2(-1,-1), Vec2(1,-1),
		Vec2(-2,-2) , Vec2(0, -2), Vec2(2,-2)
	};

	std::vector<float> const blurDownWeights = {
		0.0323f, 0.0645f, 0.0323f,
		0.1290f, 0.1290f,
		0.0645f, 0.0968f, 0.0645f,
		0.1290f, 0.1290f,
		0.0323f, 0.0645f, 0.0323f
	};

	BindShader(GetOrCreateShader("Blur", VertexType::PCU));
	SetBlendMode(BlendMode::OPAQUE);

	for (int i = 0; i < m_emissiveRenderTargets.size() - 1; ++i)
	{
		Vec2 texelSize = Vec2(1.f / m_emissiveRenderTargets[i + 1]->m_dimensions.x, 1.f / m_emissiveRenderTargets[i + 1]->m_dimensions.y);

		D3D11_VIEWPORT view = {};
		view.TopLeftX = 0;
		view.TopLeftY = 0;
		view.Width = (FLOAT)m_emissiveRenderTargets[i + 1]->m_dimensions.x;
		view.Height = (FLOAT)m_emissiveRenderTargets[i + 1]->m_dimensions.y;
		view.MinDepth = 0;
		view.MaxDepth = 1;
		m_deviceContext->RSSetViewports(1, &view);

		SetRenderTarget(m_emissiveRenderTargets[i + 1]);
		BindTexture(m_emissiveRenderTargets[i], 0);
		BindTexture(nullptr, 1);
		SetBlurConstants(texelSize, 0.f, blurDownOffsets, blurDownWeights);
		DrawVertexBuffer(m_fullscreenVBO, 6, 0);
	}

	std::vector<Vec2> upsampleOffsets{
		Vec2(-1, 1), Vec2(0,1), Vec2(1,1),
		Vec2(-1,0), Vec2(0,0), Vec2(1,0),
		Vec2(-1,-1), Vec2(0,-1), Vec2(1,-1)
	};

	std::vector<float> upsampleWeights
	{
		0.0625, 0.125, 0.0625,
		0.125, 0.25, 0.125,
		0.0625, 0.125, 0.0625
	};

	for (size_t i = m_emissiveRenderTargets.size() - 1; i > 0; --i)
	{
		Vec2 texelSize = Vec2(1.f / m_emissiveRenderTargetsUpscale[i - 1]->m_dimensions.x, 1.f / m_emissiveRenderTargetsUpscale[i - 1]->m_dimensions.y);

		D3D11_VIEWPORT view = {};
		view.TopLeftX = 0;
		view.TopLeftY = 0;
		view.Width = (FLOAT)m_emissiveRenderTargetsUpscale[i - 1]->m_dimensions.x;
		view.Height = (FLOAT)m_emissiveRenderTargetsUpscale[i - 1]->m_dimensions.y;
		view.MinDepth = 0;
		view.MaxDepth = 1;
		m_deviceContext->RSSetViewports(1, &view);

		SetRenderTarget(m_emissiveRenderTargetsUpscale[i - 1]);
		BindTexture(m_emissiveRenderTargets[i], 0);
		if (i == m_emissiveRenderTargets.size() - 1)
		{
			BindTexture(m_emissiveRenderTargets[m_emissiveRenderTargets.size() - 1], 1);
		}
		else
		{
			BindTexture(m_emissiveRenderTargetsUpscale[i], 1);
		}
		SetBlurConstants(texelSize, 0.5f, upsampleOffsets, upsampleWeights);
		DrawVertexBuffer(m_fullscreenVBO, 6, 0);
	}
	
	IntVec2 viewDims = m_config.m_window->GetClientDimensions();

	D3D11_VIEWPORT view = {};
	view.TopLeftX = 0;
	view.TopLeftY = 0;
	view.Width = (FLOAT)viewDims.x;
	view.Height = (FLOAT)viewDims.y;
	view.MinDepth = 0;
	view.MaxDepth = 1;
	m_deviceContext->RSSetViewports(1, &view);

	BindShader(GetOrCreateShader("Composite"));
	SetBlendMode(BlendMode::ADDITIVE);
	BindTexture(m_emissiveRenderTargetsUpscale[1], 0);
	SetRenderTarget(nullptr);
	DrawVertexBuffer(m_fullscreenVBO, 6, 0);

	SetBlendMode(BlendMode::ALPHA);
	SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
}