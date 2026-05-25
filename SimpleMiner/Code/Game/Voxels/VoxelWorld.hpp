#pragma once

#include "Engine/Math/RaycastResult.hpp"
#include "Engine/Threads/JobSystem.hpp"

#include "Game/Voxels/BlockDef.hpp"
#include "Game/Voxels/Chunk.hpp"
#include "Game/Voxels/BlockIterator.hpp"

#include <deque>

class Game;

class LightingJob : public Job
{
public:
	LightingJob(Chunk* chunk) : m_chunk(chunk) {};
	Chunk* m_chunk = nullptr;
	virtual void Execute() override;
};


enum BlockFace
{
	BLOCK_FACE_NORTH,
	BLOCK_FACE_SOUTH,
	BLOCK_FACE_WEST,
	BLOCK_FACE_EAST,
	BLOCK_FACE_SKYWARD,
	BLOCK_FACE_GROUNDWARD
};

class ChunkCoordComparator
{
public:
	bool operator()(IntVec2 const& a , IntVec2 const& b) const
	{
		if (a.y < b.y)
		{
			return true;
		}
		else if (a.y == b.y)
		{
			return a.x < b.x;
		}
		else
		{
			return false;
		}
	}
};

class VoxelWorld
{
public:
	JobSystem m_ioJobSystem;
public:
	VoxelWorld(Game* game, uint32_t seed);
	~VoxelWorld();

	BlockDefRegistry m_blockDefs;
	void Startup();

	void Update(Vec3 viewerPos);
	void Render();

	Chunk* GetChunkAt(IntVec2 chunkPos) const;
	bool HasChunkAtChunkPos(IntVec2 chunkPos) const;

	void  SetBlock(IntVec3 globalBlockPos, Block block);
	Block* GetBlock(IntVec3 globalBlockPos) const;

	void MarkDirtyLighting(BlockIterator const& itr);
	void UndirtyAllLightsInChunk(IntVec2 chunkPos);
	RaycastResult3D RaycastVsBlocks(Vec3 const& begin, Vec3 const& fwdNormal, float distance) const;
private:
	uint32_t m_seed;
	int m_activeChunkCount = 0;

	Game* m_game;
	std::deque<BlockIterator> m_dirtyLights;

	std::mutex m_lightingMutex;
	std::vector<LightingJob> m_lightingJobsInFlight;
	size_t m_dirtyLightSwapIndex = 0;
	std::vector<BlockIterator> m_dirtyLightSwapBuffers[2];

	std::map<IntVec2, Chunk*,ChunkCoordComparator> m_chunks;
private:
	void ProcessAllDirtyLighting();
	void RegisterBlockDefs();
};