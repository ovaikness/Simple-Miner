#include "Game/Voxels/BlockIterator.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Game/Voxels/VoxelCommon.hpp"
#include "Game/Voxels/Chunk.hpp"

BlockIterator::BlockIterator(Chunk* chunk, int index)
	: m_chunk(chunk)
	, m_index(index)
{

}

Vec3 BlockIterator::GetWorldCenter() const
{
	if (m_chunk)
	{
		IntVec2 chunkCoords			= m_chunk->GetChunkCoords();
		IntVec3 worldChunkCoords    = ChunkCoordsToGlobalCoords(chunkCoords);
		IntVec3 localBlockPos		= GetLocalBlockCoordsFromIndex(m_index);
		IntVec3 globalBlockPos		= worldChunkCoords + localBlockPos;

		return Vec3(globalBlockPos.x + 0.5f, globalBlockPos.y + 0.5f, globalBlockPos.z + 0.5f);
	}
	else
	{
		ERROR_AND_DIE("Attempted to get world center of invalid block iterator!");
	}
}

Block* BlockIterator::GetBlock() const
{
	if (m_chunk)
	{
		if (m_chunk->m_blocks)
		{
			return &m_chunk->m_blocks[m_index];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}

BlockIterator BlockIterator::GetEastNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.y == CHUNK_Y_SIZE - 1)
	{
		result.m_chunk = m_chunk->m_neighorPosY;
		localBlockPos.y = 0;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.y += 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator BlockIterator::GetWestNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.y == 0)
	{
		result.m_chunk = m_chunk->m_neighorNegY;
		localBlockPos.y = CHUNK_Y_SIZE - 1;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.y -= 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator BlockIterator::GetSkywardNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.z == CHUNK_Z_SIZE - 1)
	{
		result.m_chunk = nullptr;
		localBlockPos.z = 0;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.z += 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator BlockIterator::GetGroundwardNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.z == 0)
	{
		result.m_chunk = nullptr;
		localBlockPos.z = CHUNK_Z_SIZE - 1;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.z -= 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator BlockIterator::GetNorthNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.x == CHUNK_X_SIZE- 1)
	{
		result.m_chunk = m_chunk->m_neighorPosX;
		localBlockPos.x = 0;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.x += 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator BlockIterator::GetSouthNeighbor() const
{
	BlockIterator result;
	IntVec3 localBlockPos = GetLocalBlockCoordsFromIndex(m_index);
	if (localBlockPos.x == 0)
	{
		result.m_chunk = m_chunk->m_neighorNegX;
		localBlockPos.x = CHUNK_X_SIZE - 1;
	}
	else
	{
		result.m_chunk = m_chunk;
		localBlockPos.x -= 1;
	}
	result.m_index = GetIndexFromLocalBlockCoords(localBlockPos);
	return result;
}

BlockIterator::BlockIterator()
{
}

