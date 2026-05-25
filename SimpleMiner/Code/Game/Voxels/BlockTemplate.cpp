#include "Game/Voxels/BlockTemplate.hpp"
#include "Game/Voxels/Chunk.hpp"

std::map<std::string, BlockTemplate> BlockTemplate::s_blockTemplates;

BlockTemplate::BlockTemplate()
{
}

void BlockTemplate::GenerateIntoChunk(Chunk* chunk, IntVec3 pos) const
{
	for (BlockTemplateEntry const& entry : m_entries)
	{
		IntVec3 offsetPos = pos + entry.m_offset;

		if (offsetPos.x >= 0 && offsetPos.x < CHUNK_X_SIZE &&
			offsetPos.y >= 0 && offsetPos.y < CHUNK_Y_SIZE &&
			offsetPos.z >= 0 && offsetPos.z < CHUNK_Z_SIZE)
		{
			int index = GetIndexFromLocalBlockCoords(offsetPos);
			chunk->m_blocks[index] = entry.m_block;
		}
	}
}

void BlockTemplate::PushEntry(BlockTemplateEntry const& entry)
{
	m_entries.push_back(entry);
}

void BlockTemplate::PushEntry(BlockTemplateEntry&& entry)
{
	m_entries.push_back(entry);
}

