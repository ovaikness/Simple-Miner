#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/Vertex_Font.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>
#include <unordered_map>
#ifdef OPAQUE
#undef OPAQUE
#endif
template<typename T>
constexpr void SafeRelease(T*& dxObject)
{
	if (dxObject != nullptr)
	{
		dxObject->Release();
		dxObject = nullptr;
	}
}

class Shader;
class Window;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

struct LightDebug
{
	int RenderAmbient = 1;
	int RenderDirectional = 1;
	int RenderSpecular = 1;
	int RenderEmissive = 1;
	int UseDiffuseMap = 1;
	int UseNormalMap = 1;
	int UseSpecularMap = 1;
	int UseGlossinessMap = 1;
	int UseEmissiveMap = 1;

	float padding[3];
};

struct RenderConfig
{
	Window* m_window = nullptr;
	bool m_enableEmissive = false;
};

enum class VertexType
{
	PCU,
	PCUTBN,
	FONT,
	COUNT
};

enum class PrimitiveMode
{
	TRIANGLES,
	LINES,
};

enum class DepthMode
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_WRITE_LESS_EQUAL,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	BILINEAR_CLAMP,
	COUNT
};

enum class BlendMode
{
	ADDITIVE,
	ALPHA,
	OPAQUE,
	COUNT
};

class Renderer
{
public:
	// Light Configurations
	float m_ambientIntensity = 0.35f;
	float m_sunIntensity = 0.85f;
	Vec3  m_sunDirection = Vec3(2.f, 1.f, -1.f);
	Vec3  m_worldEyePos = Vec3(0.f, 0.f, 0.f);
	float m_minFalloff = 0.1f;
	float m_maxFalloff = 0.2f;
	float m_specularPower = 16.f;
	float m_specularFactor = 1.f;
	LightDebug m_lightDebug;

	PrimitiveMode m_primitiveMode = PrimitiveMode::TRIANGLES;
public:
	Renderer();
	Renderer(RenderConfig const& config);

	void InitDepthStencilStates();
	void InitRasterizerStates();
	void InitSamplerStates();
	void InitBlendStates();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera(const Camera& camera);
	void EndCamera(const Camera& camera);
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray(int numVertexes, const Vertex_Font* vertexes);

	void SetStatesIfChanged();

	Camera const* GetBoundCamera() const;

	BitmapFont* CreateOrGetBitmapFont(char const* bitmapFontPath);
	BitmapFont* CreateBitmapFontFromFile(char const* bitmapFontPath);
	BitmapFont* GetBitmapFontForFilename(char const* bitmapFontPath);
	Texture* GetTextureForFileName(char const* imageFilePath);
	Texture* CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture* CreateRenderTexture(IntVec2 dimensions, char const* name);

	Texture* CreateTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);

	ConstantBuffer* CreateConstantBuffer(size_t const size);
	void CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);

	void BindTexture(Texture const* texture, int slot = 0);
	void DrawVertexArray( std::vector<Vertex_PCU> const& vertices );
	void DrawVertexArray( std::vector<Vertex_Font> const& vertices );
	void SetRasterizerMode(RasterizerMode mode);
	void SetSamplerMode(SamplerMode mode);
	void SetBlendMode(BlendMode mode);
	void SetDepthMode(DepthMode mode);

	void SetLightConstants(
		Vec3 const& sunDirection,
		float sunIntensity,
		float ambientIntensity,
		Vec3 worldEyePos,
		float minFalloff,
		float maxFalloff,
		float specularPower,
		float specularFactor,
		LightDebug const& lightDebug
	);

	void SetGameConstants(float time);
	void SetModelConstants(Mat44 const& modelMatrix = Mat44::IDENTITY, Rgba8 const& modelColor = Rgba8::WHITE);
	void SetMinerConstants(Vec3 const& position, Rgba8 const& indoorColor, Rgba8 const& outdoorColor, Rgba8 const& skyColor, float fogNear, float fogFar);
	void SetBlurConstants(Vec2 const& texelSize, float lerpT, std::vector<Vec2> const& sampleOffsets, std::vector<float> const& sampleWeights);

	Shader* GetOrCreateShader(char const* shaderName, VertexType = VertexType::PCU);
	Shader* CreateShader(char const* shaderName, VertexType type = VertexType::PCU);
	Shader* CreateShader(char const* shaderName, char const* shaderSource, VertexType type = VertexType::PCU);
	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
		char const* source, char const* entryPoint, char const* target);
	void BindShader(Shader* shader);

	VertexBuffer* CreateVertexBufferFromVertexPCUTBNArray(std::vector<Vertex_PCUTBN> const& verts);
	VertexBuffer* CreateVertexBufferFromVertexPCUArray(std::vector<Vertex_PCU> const& verts);
	VertexBuffer* CreateVertexBuffer(size_t const size);

	IndexBuffer* CreateIndexBuffer(std::vector<unsigned int> const& data);
	IndexBuffer* CreateIndexBuffer(size_t const size, unsigned int const* data);

	void CopyCPUToGPU(void const* data, size_t size, VertexBuffer* vbo);
	void CopyCPUToGPU(void const* data, size_t size, IndexBuffer* ibo);

	void SetPrimitiveTopology(PrimitiveMode mode);
	void BindVertexBuffer(VertexBuffer* vbo, VertexType type = VertexType::PCU);
	void BindIndexBuffer(IndexBuffer* ibo);
	void DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, int vertexOffset = 0, VertexType vertexType = VertexType::PCU);
	void DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, int indexCount, int indexOffset = 0, int vertexOffset = 0, VertexType type = VertexType::PCUTBN);

	void SetRenderTarget(Texture* texture);
	void ClearRenderTargetTexture(Texture* texture);

	void CreateEmissiveRenderTargets();

	void RenderEmissive();
protected:
	mutable VertexBuffer* m_immediateVBO = nullptr;
	VertexBuffer* m_fullscreenVBO = nullptr;
	Camera const*	m_boundCamera = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;
	ConstantBuffer* m_lightCBO = nullptr;
	ConstantBuffer* m_minerCBO = nullptr;
	ConstantBuffer* m_blurCBO = nullptr;
	ConstantBuffer* m_gameCBO = nullptr;

	std::vector<Texture*> m_emissiveRenderTargets;
	std::vector<Texture*> m_emissiveRenderTargetsUpscale;

	Texture const*  m_defaultTexture = nullptr;
	Texture const* m_currentTexture = nullptr;
	int			   m_currentSlot = 0;

	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilTexture = nullptr;

	ID3D11DepthStencilState* m_depthState = nullptr;
	DepthMode m_desiredDepthMode = DepthMode::DISABLED;
	ID3D11DepthStencilState* m_depthStates[(size_t)(DepthMode::COUNT)];

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	RasterizerMode m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	ID3D11RasterizerState* m_rasterizerStates[(size_t)(RasterizerMode::COUNT)];

	ID3D11SamplerState* m_samplerState = nullptr;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState* m_samplerStates[(size_t)(SamplerMode::COUNT)] = {};

	ID3D11BlendState* m_blendState = nullptr;
	BlendMode		  m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[(size_t)(BlendMode::COUNT)] = {};

	std::vector<Shader*> m_loadedShaders;

	Shader* m_defaultShader = nullptr;
	Shader* m_currentShader = nullptr;

	std::unordered_map<std::string, BitmapFont*> m_loadedBitmapFonts;
	std::unordered_map<std::string, Texture*>    m_loadedTextures;
	RenderConfig m_config;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;

#if defined(ENGINE_DEBUG_RENDERER)
	void* m_dxgiDebug = nullptr;
	void* m_dxgiDebugModule = nullptr;
#endif
protected:
	//void CreateRenderingContext();
};

