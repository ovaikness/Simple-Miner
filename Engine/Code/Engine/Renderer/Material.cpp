#include "Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

bool Material::LoadMtl(Renderer& renderer, std::filesystem::path filePath)
{
	std::string fileString;
	int result = FileReadToString(fileString, filePath.string());
	if (result == -1)
	{
		return false;
	}

	std::vector<std::string> lines = SplitStringOnDelimiter(fileString, '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		if (tokens[0] == "map_Kd")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_diffuseTexture = texture;
		}
		else if (tokens[0] == "map_Bump")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_normalTexture = texture;
		}
		else if (tokens[0] == "map_Ke")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_emissiveTexture = texture;
		}
		else if (tokens[0] == "Kd")
		{
			m_diffuseColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ks")
		{
			m_specularColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ke")
		{
			m_emissiveColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ns")
		{
			m_specularPower = std::stof(tokens[1]);
		}
	}

    return 0;
}

bool Material::LoadMtlData(Renderer& renderer, std::string_view data)
{
	std::vector<std::string> lines = SplitStringOnDelimiter(std::string(data), '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		if (tokens[0] == "map_Kd")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_diffuseTexture = texture;
		}
		else if (tokens[0] == "map_Bump")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_normalTexture = texture;
		}
		else if (tokens[0] == "map_Ke")
		{
			Texture* texture = nullptr;
			std::filesystem::path texturePath = texturePath / tokens[1];
			texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
			m_emissiveTexture = texture;
		}
		else if (tokens[0] == "Kd")
		{
			m_diffuseColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ks")
		{
			m_specularColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ke")
		{
			m_emissiveColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ns")
		{
			m_specularPower = std::stof(tokens[1]);
		}
	}

	return 0;
}
