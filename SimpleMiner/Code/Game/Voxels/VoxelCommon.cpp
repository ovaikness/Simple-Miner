#include "Game/Voxels/VoxelCommon.hpp"

IntVec3 ChunkCoordsToGlobalCoords(IntVec2 const& chunkCoords)
{
	return IntVec3(chunkCoords.x * CHUNK_X_SIZE, chunkCoords.y * CHUNK_Y_SIZE,0);
}

IntVec3 GetLocalBlockPosFromGlobalBlockPos(IntVec3 globalBlockPos)
{
	int x = globalBlockPos.x - (globalBlockPos.x / CHUNK_X_SIZE) * 16;
	int y = globalBlockPos.y - (globalBlockPos.y / CHUNK_Y_SIZE) * 16;

	return IntVec3(x, y, globalBlockPos.z);
}

IntVec3 GetLocalBlockCoordsFromIndex(int index)
{
	return IntVec3(GetLocalBlockXFromIndex(index), GetLocalBlockYFromIndex(index), GetLocalBlockZFromIndex(index));
}

int GetIndexFromLocalBlockCoords(IntVec3 localBlockPos)
{
	int xx = (localBlockPos.x & CHUNK_X_MASK) << CHUNK_X_SHIFT;
	int yy = (localBlockPos.y & CHUNK_Y_MASK) << CHUNK_Y_SHIFT;
	int zz = (localBlockPos.z & CHUNK_Z_MASK) << CHUNK_Z_SHIFT;
	return xx | yy | zz;
}
