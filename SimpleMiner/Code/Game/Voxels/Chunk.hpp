#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Threads/JobSystem.hpp"

#include "Game/Voxels/VoxelCommon.hpp"
#include "Game/Voxels/Block.hpp"
#include "Game/GameCommon.hpp"

#include <vector>

class VoxelWorld;
class Chunk;
class BlockIterator;

class ChunkMeshBuildJob : public Job
{
public:
	Chunk* m_chunk = nullptr;
	virtual void Execute() override;
};

class ChunkSaveJob : public Job
{
public:
	Chunk* m_chunk = nullptr;
	virtual void Execute() override;
};

class ChunkLoadJob : public Job
{
public:
	Chunk* m_chunk = nullptr;
	virtual void Execute() override;
};

class ChunkGenerationJob : public Job
{
public:
	Chunk* m_chunk = nullptr;
	public:
	virtual void Execute() override;
};

enum class ChunkState
{
	DISABLED,
	BUILDING_MESH,
	QUEUED_FOR_GENERATION,
	QUEUED_FOR_LOAD,
	QUEUED_FOR_SAVING,
	LOADING,
	SAVING,
	GENERATING,
	COMPLETE,
	ACTIVE

};

class Chunk
{
public:
	size_t m_dirtyLightSwapIndex = 0;
	std::vector<BlockIterator> m_dirtyLightSwapBuffer[2];
	std::mutex m_lightBackBufferMutex;
	bool m_processingLighting = false;
	void ProcessDirtyLighting();
	void PushLightToBackBuffer(BlockIterator const& iter);
	void SwapLightBuffers();
public:
	Chunk(VoxelWorld* world, IntVec2 coords, uint32_t seed);
	~Chunk();
	void Startup();

	void Activate();
	void Deactivate();

	bool IsActive() const;

	void StartMeshBuildJob();
	void StartLoadJob();
	void StartGenerationJob();
	void GenerateChunkData();
	void DirtyLightsAfterGeneration();
	void GenerateMesh();

	Block GetBlockFromLocalBlockPos(IntVec3 blockPos);

	void  SetBlock(IntVec3 localBlockCoords, Block block);
	Block* GetBlock(IntVec3 localBlockCoords) const;
	void DebugRender();
	void Update();
	void Render();

	bool AllNeighborsAreNotProcessingLighting() const;

	void AddNeighborPosX(Chunk* chunk);
	void AddNeighborNegX(Chunk* chunk);
	void AddNeighborPosY(Chunk* chunk);
	void AddNeighborNegY(Chunk* chunk);

	void RemoveNeighborPosX();
	void RemoveNeighborNegX();
	void RemoveNeighborPosY();
	void RemoveNeighborNegY();

	IntVec2 GetChunkCoords();

	std::string GetFilePath();

	void SaveToFile();
	bool LoadFromData(std::vector<uint8_t> const& data);
public:
	uint32_t m_seed = 0;
	int m_activeChunks = 0;

	std::atomic<ChunkState> m_state = ChunkState::DISABLED;

	bool  m_hasAllNeighbors = false;
	bool  m_dirtyData = false;
	bool  m_dirtyMesh = false;
	AABB3 m_bounds;
	IntVec2 m_chunkCoords;

	VoxelWorld* m_voxelWorld = nullptr;

	ChunkGenerationJob*   m_generationJob	= nullptr;

	ChunkMeshBuildJob*    m_meshBuildJob    = nullptr;
	ChunkLoadJob*         m_loadJob			= nullptr;
	ChunkSaveJob*         m_saveJob			= nullptr;

	Chunk* m_neighorPosX = nullptr;
	Chunk* m_neighorNegX = nullptr;
	Chunk* m_neighorPosY = nullptr;
	Chunk* m_neighorNegY = nullptr;

	Block* m_blocks = nullptr;

protected:
	bool m_active = false;
	std::vector<Vertex_PCU> m_vertices;
	VertexBuffer* m_vbo = nullptr;
	int m_vertexCount = 0;
protected:
	void StartGenerateOrLoad();
};