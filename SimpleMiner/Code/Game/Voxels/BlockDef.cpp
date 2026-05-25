#include "Game/Voxels/BlockDef.hpp"

#include "Engine/Core/EngineCommon.hpp"

BlockDefRegistry::BlockDefRegistry()
{
	//BlockDef def;
	//def.m_name = "null";
	//def.m_isVisible = false;
	//RegisterBlockDef(def);
}

BlockDefRegistry::~BlockDefRegistry()
{
}

void BlockDefRegistry::LoadBlockDefsFromFile(std::string const& filePath)
{
	UNUSED(filePath);
	//#TODO: Implement
}

void BlockDefRegistry::SetSpriteSheet(SpriteSheet const& spriteSheet)
{
	m_spriteSheet = spriteSheet;
}

SpriteSheet const& BlockDefRegistry::GetSpriteSheet() const
{
	return m_spriteSheet;
}

std::string BlockDefRegistry::GetBlockNameFromType(int index) const
{
	if (index < 0 || index > m_blockDefList.size() - 1)
	{
		return "Unknown";
	}

	BlockDef def = m_blockDefList[index];
	return def.m_name;
}

Block BlockDefRegistry::GetBlockByName(std::string const& name) const
{
	return m_blockByName.at(name);
}

BlockDef const& BlockDefRegistry::GetBlockDefByName(std::string const& name) const
{
	return m_blockDefList[GetBlockByName(name).m_type];
}

BlockDef const& BlockDefRegistry::GetBlockDefByType(int index) const
{
	return m_blockDefList[index];
}

void BlockDefRegistry::RegisterBlockDef(BlockDef const& blockDef)
{
	Block block;
	block.m_type = static_cast<char>(m_blockDefList.size());

	m_blockByName[blockDef.m_name] = block;
	m_blockDefList.push_back(blockDef);
}

BlockDefRegistry* BlockDef::s_blockDefRegistry = new BlockDefRegistry();