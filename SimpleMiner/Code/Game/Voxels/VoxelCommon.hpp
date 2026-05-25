#pragma once
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/IntVec2.hpp"

enum VoxelFace
{
	VOXEL_FACE_FRONT,
	VOXEL_FACE_BACK,
	VOXEL_FACE_LEFT,
	VOXEL_FACE_RIGHT,
	VOXEL_FACE_TOP,
	VOXEL_FACE_BOTTOM,
	VOXEL_FACE_COUNT
};

constexpr int CHUNK_X_BITS = 4;
constexpr int CHUNK_Y_BITS = 4;
constexpr int CHUNK_Z_BITS = 7;

constexpr size_t CHUNK_X_SIZE = 1 << CHUNK_X_BITS;
constexpr size_t CHUNK_Y_SIZE = 1 << CHUNK_Y_BITS;
constexpr size_t CHUNK_Z_SIZE = 1 << CHUNK_Z_BITS;

constexpr int CHUNK_X_MASK = CHUNK_X_SIZE - 1;
constexpr int CHUNK_Y_MASK = CHUNK_Y_SIZE - 1;
constexpr int CHUNK_Z_MASK = CHUNK_Z_SIZE - 1;

constexpr int CHUNK_X_SHIFT = CHUNK_Z_BITS;
constexpr int CHUNK_Y_SHIFT = CHUNK_Z_BITS + CHUNK_X_BITS;
constexpr int CHUNK_Z_SHIFT = 0;

constexpr size_t CHUNK_TOTAL_VOXELS = CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE;

IntVec3 ChunkCoordsToGlobalCoords(IntVec2 const& chunkCoords);

IntVec3 GetLocalBlockPosFromGlobalBlockPos(IntVec3 globalBlockPos);

constexpr int GetLocalBlockXFromIndex(int index)
{
	return (index >> CHUNK_X_SHIFT) & CHUNK_X_MASK;
}

constexpr int GetLocalBlockYFromIndex(int index)
{
	return (index >> CHUNK_Y_SHIFT) & CHUNK_Y_MASK;
}

constexpr int GetLocalBlockZFromIndex(int index)
{
	return (index >> CHUNK_Z_SHIFT) & CHUNK_Z_MASK;
}

IntVec3 GetLocalBlockCoordsFromIndex(int index);
int GetIndexFromLocalBlockCoords(IntVec3 localBlockPos);