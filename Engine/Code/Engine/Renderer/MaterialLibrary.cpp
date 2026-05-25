#include "Engine/Renderer/MaterialLibrary.hpp"
#include "Engine/Core/FileUtils.hpp"

MaterialLibrary::MaterialLibrary()
{
	Material* material = new Material();
	material->m_shader = nullptr;
	m_materials["Main"] = material;
}

MaterialLibrary::~MaterialLibrary()
{
	for (auto const& material : m_materials)
	{
		delete material.second;
	}
	m_materials.clear();
}

void MaterialLibrary::LoadMtl(Renderer& renderer, std::filesystem::path path)
{
	for (auto const& material : m_materials)
	{
		delete material.second;
	}
	m_materials.clear();
	std::string data;
	int result = FileReadToString(data, path.string());
	if (result == -1)
	{
		return;
	}

	std::vector<std::string> lines = SplitStringOnDelimiter(data, '\n');
	Material* currentMaterial = nullptr;
	std::filesystem::path textureDir = "Data/Textures/";
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');

		if (tokens[0] == "newmtl")
		{
			currentMaterial = new Material();
			m_materials[tokens[1]] = currentMaterial;
		}

		if (currentMaterial)
		{
			if (tokens[0] == "map_Kd")
			{
				Texture* texture = nullptr;
				std::filesystem::path texturePath = textureDir / tokens[1];
				texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
				currentMaterial->m_diffuseTexture = texture;
			}
			else if (tokens[0] == "map_Bump")
			{
				Texture* texture = nullptr;
				std::filesystem::path texturePath = textureDir / tokens[1];
				texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
				currentMaterial->m_normalTexture = texture;
			}
			else if (tokens[0] == "map_Ke")
			{
				Texture* texture = nullptr;
				std::filesystem::path texturePath = textureDir / tokens[1];
				texture = renderer.CreateOrGetTextureFromFile(texturePath.string().c_str());
				currentMaterial->m_emissiveTexture = texture;
			}
			else if (tokens[0] == "Kd")
			{
				currentMaterial->m_diffuseColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			}
			else if (tokens[0] == "Ks")
			{
				currentMaterial->m_specularColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			}
			else if (tokens[0] == "Ke")
			{
				currentMaterial->m_emissiveColor = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			}
			else if (tokens[0] == "Ns")
			{
				currentMaterial->m_specularPower = std::stof(tokens[1]);
			}
		}
	}
}

bool MaterialLibrary::LoadXML(Renderer& renderer, std::filesystem::path path)
{
	XmlDocument doc;
	if (doc.LoadFile(path.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		return false;
	}
	XmlElement* root = doc.FirstChildElement();

	if (!root)
	{
		return false;
	}

	std::string shaderStr;
	shaderStr       = ParseXMLAttribute(*root, "shader", "Default");

	std::filesystem::path shaderPath = shaderStr;
	shaderPath = shaderPath.filename();

	std::string vertexTypeStr;
	vertexTypeStr   = ParseXMLAttribute(*root, "vertexType", "Vertex_PCUTBN");

	std::string diffuseTexStr;
	diffuseTexStr   = ParseXMLAttribute(*root, "diffuseTexture", "");

	std::string normalTexStr;
	normalTexStr    = ParseXMLAttribute(*root, "normalTexture", "");

	std::string specGlossTexStr;
	specGlossTexStr = ParseXMLAttribute(*root, "specGlossEmitTexture", "");

	VertexType vertexType;
	if (vertexTypeStr == "Vertex_PCUTBN")
	{
		vertexType = VertexType::PCUTBN;
	}
	else if (vertexTypeStr == "Vertex_PCUT")
	{
		vertexType = VertexType::PCU;
	}


	Material* material = new Material();
	if (std::filesystem::exists(shaderPath))
	{
		material->m_shader = renderer.GetOrCreateShader(shaderPath.string().c_str());
	}
	else
	{
		material->m_shader = nullptr;
	}

	if (std::filesystem::exists(diffuseTexStr))
	{
		material->m_diffuseTexture = renderer.CreateOrGetTextureFromFile(diffuseTexStr.c_str());
	}
	else
	{
		material->m_diffuseTexture = nullptr;
	}

	if (std::filesystem::exists(normalTexStr))
	{
		material->m_normalTexture = renderer.CreateOrGetTextureFromFile(normalTexStr.c_str());
	}
	else
	{
		material->m_normalTexture = nullptr;
	}

	if (std::filesystem::exists(specGlossTexStr))
	{
		material->m_specularGlossTexture = renderer.CreateOrGetTextureFromFile(specGlossTexStr.c_str());
	}
	else
	{
		material->m_specularGlossTexture = nullptr;
	}

	m_materials["Main"] = material;

	return true;
}

Material* MaterialLibrary::GetMaterial(std::string const& name) const
{
	auto found = m_materials.find(name);
	if (found != m_materials.end())
	{
		return found->second;
	}
	if (m_materials.size() == 0)
	{
		return nullptr;
	}
	// Return the first material in the map if the name is not found (this is the main material)
	return m_materials.begin()->second;
}
