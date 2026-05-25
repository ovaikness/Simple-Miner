#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include <filesystem>
#include <string_view>

class Material
{
public:
	bool LoadMtl(Renderer& renderer, std::filesystem::path filePath);
	bool LoadMtlData(Renderer& renderer, std::string_view data );
public:
	Vec3 m_diffuseColor = Vec3(1.f,1.f,1.f);
	Vec3 m_specularColor = Vec3(1.f,1.f,1.f);
	Vec3 m_emissiveColor = Vec3(1.f,1.f,1.f);
	float m_specularPower = 0.f;

	Shader*  m_shader = nullptr;
	VertexType m_vertexType = VertexType::PCUTBN;
	Texture* m_diffuseTexture = nullptr;
	Texture* m_normalTexture = nullptr;
	Texture* m_emissiveTexture = nullptr;
	Texture* m_specularGlossTexture = nullptr;

	std::string m_shaderName;
	std::string m_diffuseTexturePath;
	std::string m_normalTexturePath;
	std::string m_emissiveTexturePath;
	std::string m_specularGlossTexturePath;

	Rgba8 m_tint = Rgba8::WHITE;
};