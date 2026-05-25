#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/Voxels/Block.hpp"
#include <vector>
#include <map>
#include <string>
#include <string_view>

class BlockDefRegistry;

class BlockDef
{
public:
	std::string m_name = "Air";
	bool m_isSolid = false;
	bool m_isOpaque = false;
	bool m_isVisible = false;
	SpriteSheet* m_spriteSheet = nullptr;
	IntVec2 m_sideTexCoords = IntVec2(0, 0);
	IntVec2 m_topTexCoords = IntVec2(0, 0);
	IntVec2 m_bottomTexCoords = IntVec2(0, 0);
	uint8_t m_lightLevel = 0;
	static BlockDefRegistry* s_blockDefRegistry;
};

class BlockDefRegistry
{
public:
	BlockDefRegistry();
	~BlockDefRegistry();

	void LoadBlockDefsFromFile(std::string const& filePath);
	void SetSpriteSheet(SpriteSheet const& spriteSheet);
	SpriteSheet const& GetSpriteSheet() const;
	std::string GetBlockNameFromType(int index) const;
	Block GetBlockByName(std::string const& name) const;
	BlockDef const& GetBlockDefByName(std::string const& name) const;
	BlockDef const& GetBlockDefByType(int index) const;
	void RegisterBlockDef(BlockDef const& blockDef);
protected:
	SpriteSheet m_spriteSheet;
	std::map<std::string, Block> m_blockByName;
	std::vector<BlockDef> m_blockDefList;
};