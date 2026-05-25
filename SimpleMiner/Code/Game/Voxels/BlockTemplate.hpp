#pragma once

#include "Engine/Math/IntVec3.hpp"

#include "Game/Voxels/BlockIterator.hpp"
#include "Game/Voxels/Block.hpp"

#include <map>
#include <vector>
#include <string>

class BlockTemplateEntry
{
public:
	IntVec3 m_offset;
	Block   m_block;
};

class BlockTemplate
{
public:
	BlockTemplate();
	void GenerateIntoChunk(Chunk* chunk, IntVec3 localPos) const;
	void PushEntry(BlockTemplateEntry const& entry);
	void PushEntry(BlockTemplateEntry&& entry);
	static std::map<std::string, BlockTemplate> s_blockTemplates;
protected:
	std::vector<BlockTemplateEntry> m_entries;
};