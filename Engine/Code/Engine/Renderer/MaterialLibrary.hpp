#pragma once

#include "Engine/Renderer/Material.hpp"

#include <map>

class MaterialLibrary
{
	public:
	MaterialLibrary();
	~MaterialLibrary();

	void LoadMtl(Renderer& renderer, std::filesystem::path path);
	bool LoadXML(Renderer& renderer, std::filesystem::path path);

	Material* GetMaterial(std::string const& name) const;

	std::map<std::string, Material*> m_materials;
};