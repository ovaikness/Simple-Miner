#include "Game/Voxels/VoxelWorld.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"

#include "Game/Voxels/VoxelCommon.hpp"
#include "Game/Voxels/BlockTemplate.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include <chrono>

constexpr uint32_t MAX_INT = ((~0) >> 1);
constexpr int MAX_SINT = MAX_INT >> 1;

VoxelWorld::VoxelWorld(Game* game, uint32_t seed)
	: m_game(game)
	, m_ioJobSystem(1) // Reading and writing to disk is slow so we only want one thread doing it
{
	m_ioJobSystem.Startup();
	if (seed == 0)
	{
		m_seed = g_RNG.RollRandomIntLessThan(MAX_SINT) + std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}
	else
	{
		m_seed = seed;
	}
}

VoxelWorld::~VoxelWorld()
{
	for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
	{
		Chunk* chunk = chunkPair.second;
		if (chunk)
		{
			chunk->Deactivate();
		}
	}

	bool chunksStillActive = true;
	while (chunksStillActive)
	{
		chunksStillActive = false;
		for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
		{
			Chunk* chunk = chunkPair.second;
			if (chunk)
			{
				if (chunk->m_state != ChunkState::DISABLED)
				{
					chunksStillActive = true;
				}
				chunk->Update();
			}
		}
	}

	for (auto& chunk : m_chunks)
	{
		delete chunk.second;
		chunk.second = nullptr;
	}

	m_ioJobSystem.Shutdown();
}

void VoxelWorld::Startup()
{
	RegisterBlockDefs();
}

struct ChunkCoordDistComparator
{
public:
	IntVec2 m_viewChunkPos;

	ChunkCoordDistComparator(IntVec2 viewChunkPos)
		: m_viewChunkPos(viewChunkPos)
	{
	}

	bool operator()(Chunk* a, Chunk* b) const
	{
		float distA = (a->m_chunkCoords - m_viewChunkPos).GetLengthSquared();
		float distB = (b->m_chunkCoords - m_viewChunkPos).GetLengthSquared();
		return distA < distB;
	}
};

void VoxelWorld::Update(Vec3 viewerPos)
{
	//Input for debugging
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
		{
			Chunk* chunk = chunkPair.second;
			if (chunk)
			{
				chunk->Deactivate();
			}
		}
	}

	IntVec2 viewChunkPos = IntVec2(static_cast<int>(viewerPos.x) / CHUNK_X_SIZE, static_cast<int>(viewerPos.y) / CHUNK_Y_SIZE);

	int maxChunkRadiusX = 1 + static_cast<int>(m_game->m_config.m_chunkActivationRangeBlocks) / CHUNK_X_SIZE;
	int maxChunkRadiusY = 1 + static_cast<int>(m_game->m_config.m_chunkActivationRangeBlocks) / CHUNK_Y_SIZE;

	int maxChunkRadius = 1 + static_cast<int>(m_game->m_config.m_chunkActivationRangeBlocks) / CHUNK_X_SIZE;
	int maxChunks = (2 * maxChunkRadius) * (2 * maxChunkRadius);
	//Deactivate chunks not in range

	Chunk* selectedChunk = nullptr;
	if (m_activeChunkCount == maxChunks)
	{
		Chunk* chunkToDeactivate = nullptr;
		float targetDistance = 0.f;
		for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
		{
			Chunk* chunk = chunkPair.second;

			if (chunk)
			{
				if (chunk->IsActive())
				{
					float contendingDistance = (chunk->m_chunkCoords - viewChunkPos).GetLengthSquared();
					if (contendingDistance > maxChunkRadius * maxChunkRadius && contendingDistance > targetDistance)
					{
						targetDistance = contendingDistance;
						chunkToDeactivate = chunk;
					}
				}
			}
		}
		if (chunkToDeactivate)
		{
			chunkToDeactivate->Deactivate();
			--m_activeChunkCount;
		}
	}
	else if (m_activeChunkCount < maxChunks)
	{
		bool foundChunk = false;
		float targetDistance = 99999999999999999.f;
		IntVec2 coordsToGenerate;

		for (int x = -maxChunkRadiusX; x < maxChunkRadiusX; ++x)
		{
			for (int y = -maxChunkRadiusY; y < maxChunkRadiusY; ++y)
			{
				IntVec2 coords = IntVec2(x, y) + viewChunkPos;
				float contendingDistance = (coords - viewChunkPos).GetLengthSquared();
				if (contendingDistance < maxChunkRadius * maxChunkRadius)
				{
					if (contendingDistance < targetDistance)
					{
						bool needsActivation = false;
						auto chunkPair = m_chunks.find(coords);

						if (chunkPair == m_chunks.end())
						{
							needsActivation = true;
						}
						else if (chunkPair->second == nullptr)
						{
							needsActivation = true;
						}
						else if (!chunkPair->second->IsActive())
						{
							needsActivation = true;
						}

						if (needsActivation)
						{
							targetDistance = contendingDistance;
							coordsToGenerate = coords;
							foundChunk = true;
						}
					}
				}
			}
		}
		if (foundChunk)
		{
			++m_activeChunkCount;
		}

		if (m_chunks.find(coordsToGenerate) == m_chunks.end())
		{
			m_chunks[coordsToGenerate] = new Chunk(this, coordsToGenerate, m_seed);
			m_chunks[coordsToGenerate]->Activate();
		}
		else
		{
			Chunk* chunk = m_chunks.at(coordsToGenerate);
			if (chunk)
			{
				if (!chunk->IsActive())
				{
					chunk->Activate();
				}
			}
			else
			{
				chunk = new Chunk(this, coordsToGenerate, m_seed);

				m_chunks[coordsToGenerate] = chunk;
				m_chunks[coordsToGenerate]->Activate();
			}
		}
	}

	for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
	{
		Chunk* chunk = chunkPair.second;
		if (chunk->IsActive() && (chunk->m_state == ChunkState::ACTIVE || chunk->m_state == ChunkState::COMPLETE))
		{
			Chunk* upChunk = GetChunkAt(chunk->m_chunkCoords + IntVec2(0, 1));
			Chunk* downChunk = GetChunkAt(chunk->m_chunkCoords + IntVec2(0, -1));
			Chunk* leftChunk = GetChunkAt(chunk->m_chunkCoords + IntVec2(-1, 0));
			Chunk* rightChunk = GetChunkAt(chunk->m_chunkCoords + IntVec2(1, 0));

			if (upChunk)
			{
				if (upChunk->IsActive() && (upChunk->m_state == ChunkState::ACTIVE || upChunk->m_state == ChunkState::COMPLETE))
				{
					chunk->AddNeighborPosY(upChunk);
				}
			}

			if (downChunk)
			{
				if (downChunk->IsActive() && (downChunk->m_state == ChunkState::ACTIVE || downChunk->m_state == ChunkState::COMPLETE))
				{
					chunk->AddNeighborNegY(downChunk);
				}
			}

			if (leftChunk)
			{
				if (leftChunk->IsActive() && (leftChunk->m_state == ChunkState::ACTIVE || leftChunk->m_state == ChunkState::COMPLETE))
				{
					chunk->AddNeighborNegX(leftChunk);
				}
			}


			if (rightChunk)
			{
				if (rightChunk->IsActive() && (rightChunk->m_state == ChunkState::ACTIVE || rightChunk->m_state == ChunkState::COMPLETE))
				{
					chunk->AddNeighborPosX(rightChunk);
				}
			}
		}
	}

	ProcessAllDirtyLighting();

	std::set<Chunk*, ChunkCoordDistComparator> chunksToGenerateMesh(viewChunkPos);

	for (std::pair<IntVec2,Chunk*> chunkPair : m_chunks)
	{
		Chunk* chunk = chunkPair.second;
		if (chunk)
		{
			chunk->Update();
			if (chunk->m_dirtyMesh && chunk->m_hasAllNeighbors)
			{
				chunksToGenerateMesh.insert(chunk);
			}
		}
	}

	const int MAX_MESHES_PER_FRAME = 2;
	for (int i = 0; i < MAX_MESHES_PER_FRAME && i < chunksToGenerateMesh.size(); ++i)
	{
		Chunk* chunk = *chunksToGenerateMesh.begin();
		chunksToGenerateMesh.erase(chunksToGenerateMesh.begin());
		chunk->StartMeshBuildJob();
	}
}

void VoxelWorld::Render()
{
	for (auto& chunk : m_chunks)
	{
		if (chunk.second)
		{
			if (chunk.second->IsActive())
			{
				chunk.second->Render();
				if (m_game->m_debug)
				{
					chunk.second->DebugRender();
				}
			}
		}
	}
}

Chunk* VoxelWorld::GetChunkAt(IntVec2 chunkPos) const
{
	if (m_chunks.find(chunkPos) == m_chunks.end())
	{
		return nullptr;
	}
	
	return m_chunks.at(chunkPos);
}

bool VoxelWorld::HasChunkAtChunkPos(IntVec2 chunkPos) const
{
	return m_chunks.find(chunkPos) != m_chunks.end();
}

//#TODO: Handle failure on invalide positions without crashing

void VoxelWorld::SetBlock(IntVec3 globalBlockPos, Block block)
{
	IntVec2 chunkPos = IntVec2(globalBlockPos.x / CHUNK_X_SIZE, globalBlockPos.y / CHUNK_Y_SIZE);
	IntVec3 localBlockPos = GetLocalBlockPosFromGlobalBlockPos(globalBlockPos);
	if (HasChunkAtChunkPos(chunkPos) && localBlockPos.x >= 0 && localBlockPos.x < CHUNK_X_SIZE &&
		localBlockPos.y >= 0 && localBlockPos.y < CHUNK_Y_SIZE &&
		localBlockPos.z >= 0 && localBlockPos.z < CHUNK_Z_SIZE)
	{
		Chunk* chunk = m_chunks.at(chunkPos);
		if (chunk->m_state == ChunkState::ACTIVE || chunk->m_state == ChunkState::COMPLETE)
		{
			chunk->SetBlock(localBlockPos, block);
		}
	}
}

Block* VoxelWorld::GetBlock(IntVec3 globalBlockPos) const
{
	static Block air = BlockDef::s_blockDefRegistry->GetBlockByName("Air");
	IntVec2 chunkPos = IntVec2(globalBlockPos.x / CHUNK_X_SIZE, globalBlockPos.y / CHUNK_Y_SIZE);
	IntVec3 localBlockPos = GetLocalBlockPosFromGlobalBlockPos(globalBlockPos);
	if (HasChunkAtChunkPos(chunkPos) && localBlockPos.x >= 0 && localBlockPos.x < CHUNK_X_SIZE &&
		localBlockPos.y >= 0 && localBlockPos.y < CHUNK_Y_SIZE &&
		localBlockPos.z >= 0 && localBlockPos.z < CHUNK_Z_SIZE)
	{
		Chunk* chunk = m_chunks.at(chunkPos);

		if (chunk->m_state == ChunkState::COMPLETE || chunk->m_state == ChunkState::ACTIVE)
		{
			if (chunk == nullptr)
			{
				return nullptr;
			}
			else if (!chunk->IsActive())
			{
				return nullptr;
			}

			return m_chunks.at(chunkPos)->GetBlock(localBlockPos);
		}
	}
	return nullptr;
}

void VoxelWorld::MarkDirtyLighting(BlockIterator const& itr)
{
	Chunk* chunk = itr.m_chunk;
	if (chunk == nullptr)
	{
		return;
	}
	chunk->PushLightToBackBuffer(itr);
}

void VoxelWorld::ProcessAllDirtyLighting()
{
	for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
	{
		Chunk* chunk = chunkPair.second;
		if (chunk)
		{
			if (chunk->m_state == ChunkState::ACTIVE || chunk->m_state == ChunkState::COMPLETE)
			{
				chunk->SwapLightBuffers();
			}
		}
	}

	bool chunksNeedAdditionalProcessing;
	std::set<LightingJob*> lightJobsInFlight;

	do
	{
		chunksNeedAdditionalProcessing = false;
		for (std::pair<IntVec2, Chunk*> chunkPair : m_chunks)
		{
			Chunk* chunk = chunkPair.second;
			if (chunk)
			{
				if (chunk->m_state == ChunkState::ACTIVE || chunk->m_state == ChunkState::COMPLETE)
				{
					if (!chunk->m_processingLighting && chunk->AllNeighborsAreNotProcessingLighting())
					{
						//If it has dirty blocks in its front buffer only the chunk itself will reference this buffer
						//and since there should be no running jobs on this chunk we can safely process it
						if (chunk->m_dirtyLightSwapBuffer[chunk->m_dirtyLightSwapIndex].size() > 0)
						{
							chunksNeedAdditionalProcessing = true;
							chunk->m_processingLighting = true;
							chunk->m_dirtyMesh = true;
							LightingJob* job = new LightingJob(chunk);
							lightJobsInFlight.insert(job);
							g_theJobSystem->SubmitJob(job);
						}
					}
					else if (chunk->m_processingLighting)
					{
						chunksNeedAdditionalProcessing = true;
					}
				}
			}
		}

		for (auto itr = lightJobsInFlight.begin(); itr != lightJobsInFlight.end();)
		{
			if (g_theJobSystem->ClaimJobIfComplete(*itr))
			{
				LightingJob* job = *itr;
				job->m_chunk->m_processingLighting = false;
				job->m_chunk->SwapLightBuffers();
				delete job;
				itr = lightJobsInFlight.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		if (lightJobsInFlight.size() > 0)
		{
			//Instead of wasting time looping erroneously we can wait for the job system to finish a job
			g_theJobSystem->WaitUntilJobComplete(nullptr);
		}
	} while (lightJobsInFlight.size() > 0 || chunksNeedAdditionalProcessing);
}

void VoxelWorld::UndirtyAllLightsInChunk(IntVec2 chunkPos)
{
	Chunk* chunk = GetChunkAt(chunkPos);
	if (chunk == nullptr)
	{
		return;
	}

	for (std::deque<BlockIterator>::iterator itr = m_dirtyLights.begin(); itr != m_dirtyLights.end();)
	{
		BlockIterator& blockItr = *itr;
		if (blockItr.m_chunk == chunk)
		{
			itr = m_dirtyLights.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

RaycastResult3D VoxelWorld::RaycastVsBlocks(Vec3 const& start, Vec3 const& direction, float distance) const
{
	if (direction.x == 0.f && direction.y == 0.f && direction.z == 0.f)
	{
		return { false };
	}

	int tileX = (int)floor(start.x);
	int tileY = (int)floor(start.y);
	int tileZ = (int)floor(start.z);

	Block* block = GetBlock(IntVec3(tileX,tileY,tileZ));
	IntVec3 local = GetLocalBlockPosFromGlobalBlockPos(IntVec3(tileX, tileY, tileZ));
	int index = GetIndexFromLocalBlockCoords(local);
	BlockIterator blockItr(GetChunkAt(IntVec2(tileX / CHUNK_X_SIZE, tileY / CHUNK_Y_SIZE)), index);
	BlockDef def;
	if (block)
	{
		BlockDef def = block->GetBlockDef();
	}
	else
	{
		return { false };
	}

	if (def.m_isVisible)
	{
		return { true, 0.f, start, -direction , IntVec3(tileX,tileY,tileZ)};
	}

	float fwdDistPerXCrossing = 1.f / abs(direction.x);
	float fwdDistPerYCrossing = 1.f / abs(direction.y);
	float fwdDistPerZCrossing = 1.f / abs(direction.z);

	int tileStepDirectionX = direction.x < 0.f ? -1 : 1;
	int tileStepDirectionY = direction.y < 0.f ? -1 : 1;
	int tileStepDirectionZ = direction.z < 0.f ? -1 : 1;

	float yAtFirstCrossing = tileY + (tileStepDirectionY + 1.f) / 2.f;
	float xAtFirstCrossing = tileX + (tileStepDirectionX + 1.f) / 2.f;
	float zAtFirstCrossing = tileZ + (tileStepDirectionZ + 1.f) / 2.f;

	float yDistToFirstYCrossing = yAtFirstCrossing - start.y;
	float xDistToFirstXCrossing = xAtFirstCrossing - start.x;
	float zDistToFirstZCrossing = zAtFirstCrossing - start.z;

	float fwdDistAtNextYCrossing = fabsf(yDistToFirstYCrossing) * fwdDistPerYCrossing;
	float fwdDistAtNextXCrossing = fabsf(xDistToFirstXCrossing) * fwdDistPerXCrossing;
	float fwdDistAtNextZCrossing = fabsf(zDistToFirstZCrossing) * fwdDistPerZCrossing;

	for (;;)
	{
		if (fwdDistAtNextXCrossing < fwdDistAtNextYCrossing)
		{
			if (fwdDistAtNextZCrossing < fwdDistAtNextXCrossing)
			{
				if (fwdDistAtNextZCrossing > distance)
				{
					return { false };
				}

				tileZ += tileStepDirectionZ;

				if (tileStepDirectionZ == 1)
				{
					blockItr = blockItr.GetSkywardNeighbor();
				}
				else
				{
					blockItr = blockItr.GetGroundwardNeighbor();
				}

				Block* block = blockItr.GetBlock();

				if (block == nullptr)
				{
					return { false };
				}
				BlockDef tdef = block->GetBlockDef();

				if (tdef.m_isVisible)
				{
					return { true , fwdDistAtNextZCrossing, start + direction * fwdDistAtNextZCrossing, Vec3(0.f,0.f,static_cast<float>(-tileStepDirectionZ)), IntVec3(tileX,tileY,tileZ) };
				}
				else
				{
					fwdDistAtNextZCrossing += fwdDistPerZCrossing;
				}
			}
			else
			{
				if (fwdDistAtNextXCrossing > distance)
				{
					return { false };
				}

				tileX += tileStepDirectionX;

				if (tileStepDirectionX == 1)
				{
					blockItr = blockItr.GetNorthNeighbor();
				}
				else
				{
					blockItr = blockItr.GetSouthNeighbor();
				}

				Block* block = blockItr.GetBlock();
				if (block == nullptr)
				{
					return { false };
				}
				BlockDef tdef = block->GetBlockDef();

				if (tdef.m_isVisible)
				{
					return { true , fwdDistAtNextXCrossing, start + direction * fwdDistAtNextXCrossing, Vec3(static_cast<float>(-tileStepDirectionX),0.f,0.f), IntVec3(tileX,tileY,tileZ) };
				}
				else
				{
					fwdDistAtNextXCrossing += fwdDistPerXCrossing;
				}
			}
		}
		else
		{
			if (fwdDistAtNextZCrossing < fwdDistAtNextYCrossing)
			{
				if (fwdDistAtNextZCrossing > distance)
				{
					return { false };
				}

				tileZ += tileStepDirectionZ;

				if (tileStepDirectionZ == 1)
				{
					blockItr = blockItr.GetSkywardNeighbor();
				}
				else
				{
					blockItr = blockItr.GetGroundwardNeighbor();
				}

				Block* block = blockItr.GetBlock();

				if (block == nullptr)
				{
					return { false };
				}
				BlockDef tdef = block->GetBlockDef();

				if (tdef.m_isVisible)
				{
					return { true , fwdDistAtNextZCrossing, start + direction * fwdDistAtNextZCrossing, Vec3(0.f,0.f,static_cast<float>(-tileStepDirectionZ)), IntVec3(tileX,tileY,tileZ) };
				}
				else
				{
					fwdDistAtNextZCrossing += fwdDistPerZCrossing;
				}
			}
			else
			{
				if (fwdDistAtNextYCrossing > distance)
				{
					return { false };
				}

				tileY += tileStepDirectionY;
				
				if (tileStepDirectionY == 1)
				{
					blockItr = blockItr.GetEastNeighbor();
				}
				else
				{
					blockItr = blockItr.GetWestNeighbor();
				}

				Block* block = blockItr.GetBlock();

				if (block == nullptr)
				{
					return { false };
				}
				BlockDef tdef = block->GetBlockDef();

				if (tdef.m_isVisible)
				{
					return { true , fwdDistAtNextYCrossing, start + direction * fwdDistAtNextYCrossing, Vec3(0.f, static_cast<float>(-tileStepDirectionY),0.f), IntVec3(tileX,tileY,tileZ) };
				}
				else
				{
					fwdDistAtNextYCrossing += fwdDistPerYCrossing;
				}
			}
		}
	}
}

uint8_t GetMaxSkyLight(int currentLight, Block* block)
{
	if (block == nullptr)
	{
		return currentLight;
	}
	int result;
	if (block->GetSkyLight() - 1 > currentLight)
	{
		result = block->GetSkyLight() - 1;
	}
	else
	{
		result = currentLight;
	}

	if (result < 0)
	{
		return 0;
	}
	else
	{
		return static_cast<uint8_t>(result);
	}
}

uint8_t GetMaxLight(int currentLight, Block* block)
{
	if (block == nullptr)
	{
		return currentLight;
	}
	int result;
	if (block->GetBlockLight() - 1 > currentLight)
	{
		result = block->GetBlockLight() - 1;
	}
	else
	{
		result = currentLight;
	}

	if (result < 0)
	{
		return 0;
	}
	else
	{
		return static_cast<uint8_t>(result);
	}
}
void VoxelWorld::RegisterBlockDefs()
{

	BlockDef air;
	air.m_name = "Air";
	air.m_isOpaque = false;
	air.m_isSolid = false;
	air.m_isVisible = false;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(air);

	BlockDef glowstone;
	glowstone.m_name = "Glowstone";
	glowstone.m_topTexCoords = IntVec2(46, 29);
	glowstone.m_sideTexCoords = IntVec2(46, 29);
	glowstone.m_bottomTexCoords = IntVec2(46, 29);
	glowstone.m_isSolid = true;
	glowstone.m_isOpaque = true;
	glowstone.m_isVisible = true;
	glowstone.m_lightLevel = 15;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(glowstone);

	BlockDef dirt;
	dirt.m_name = "Dirt";
	dirt.m_topTexCoords = IntVec2(32, 29);
	dirt.m_sideTexCoords = IntVec2(32, 29);
	dirt.m_bottomTexCoords = IntVec2(32, 29);
	dirt.m_isSolid = true;
	dirt.m_isOpaque = true;
	dirt.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(dirt);

	BlockDef grass;
	grass.m_name = "Grass";
	grass.m_topTexCoords = IntVec2(32, 30);
	grass.m_sideTexCoords = IntVec2(33, 30);
	grass.m_bottomTexCoords = IntVec2(32, 29);
	grass.m_isSolid = true;
	grass.m_isOpaque = true;
	grass.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(grass);

	BlockDef stone;
	stone.m_name = "Stone";
	stone.m_topTexCoords = IntVec2(33, 31);
	stone.m_sideTexCoords = IntVec2(33, 31);
	stone.m_bottomTexCoords = IntVec2(33, 31);
	stone.m_isSolid = true;
	stone.m_isOpaque = true;
	stone.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(stone);

	BlockDef coal;
	coal.m_name = "Coal Ore";
	coal.m_topTexCoords = IntVec2(63, 29);
	coal.m_sideTexCoords = IntVec2(63, 29);
	coal.m_bottomTexCoords = IntVec2(63, 29);
	coal.m_isSolid = true;
	coal.m_isOpaque = true;
	coal.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(coal);

	BlockDef iron;
	iron.m_name = "Iron Ore";
	iron.m_topTexCoords = IntVec2(63, 28);
	iron.m_sideTexCoords = IntVec2(63, 28);
	iron.m_bottomTexCoords = IntVec2(63, 28);
	iron.m_isSolid = true;
	iron.m_isOpaque = true;
	iron.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(iron);

	BlockDef gold;
	gold.m_name = "Gold Ore";
	gold.m_topTexCoords = IntVec2(63, 27);
	gold.m_sideTexCoords = IntVec2(63, 27);
	gold.m_bottomTexCoords = IntVec2(63, 27);
	gold.m_isSolid = true;
	gold.m_isOpaque = true;
	gold.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(gold);

	BlockDef diamond;
	diamond.m_name = "Diamond Ore";
	diamond.m_topTexCoords = IntVec2(63, 26);
	diamond.m_sideTexCoords = IntVec2(63, 26);
	diamond.m_bottomTexCoords = IntVec2(63, 26);
	diamond.m_isSolid = true;
	diamond.m_isOpaque = true;
	diamond.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(diamond);

	BlockDef water;
	water.m_name = "Water";
	water.m_topTexCoords = IntVec2(32, 19);
	water.m_sideTexCoords = IntVec2(32, 19);
	water.m_bottomTexCoords = IntVec2(32, 19);
	water.m_isSolid = false;
	water.m_isOpaque = true;
	water.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(water);

	BlockDef bricks;
	bricks.m_name = "Bricks";
	bricks.m_topTexCoords = IntVec2(34,31);
	bricks.m_sideTexCoords = IntVec2(34,31);
	bricks.m_bottomTexCoords = IntVec2(34,31);
	bricks.m_isSolid = true;
	bricks.m_isOpaque = true;
	bricks.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(bricks);

	BlockDef cobblestone;
	cobblestone.m_name = "Cobblestone";
	cobblestone.m_topTexCoords = IntVec2(48, 31);
	cobblestone.m_sideTexCoords = IntVec2(48, 31);
	cobblestone.m_bottomTexCoords = IntVec2(48, 31);
	cobblestone.m_isSolid = true;
	cobblestone.m_isOpaque = true;
	cobblestone.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(cobblestone);

	BlockDef sand;
	sand.m_name = "Sand";
	sand.m_topTexCoords = IntVec2(34, 29);
	sand.m_sideTexCoords = IntVec2(34, 29);
	sand.m_bottomTexCoords = IntVec2(34, 29);
	sand.m_isOpaque = true;
	sand.m_isSolid = true;
	sand.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(sand);

	BlockDef ice;
	ice.m_name = "Ice";
	ice.m_topTexCoords = IntVec2(45, 29);
	ice.m_sideTexCoords = IntVec2(45, 29);
	ice.m_bottomTexCoords = IntVec2(45, 29);
	ice.m_isOpaque = true;
	ice.m_isSolid = true;
	ice.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(ice);

	BlockDef oak;
	oak.m_name = "Oak Log";
	oak.m_topTexCoords    = IntVec2(38, 30);
	oak.m_sideTexCoords   = IntVec2(36, 30);
	oak.m_bottomTexCoords = IntVec2(38, 30);
	oak.m_isOpaque = true;
	oak.m_isSolid = true;
	oak.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(oak);

	BlockDef spruce;
	spruce.m_name = "Spruce Log";
	spruce.m_topTexCoords = IntVec2(38, 30);
	spruce.m_sideTexCoords = IntVec2(37, 30);
	spruce.m_bottomTexCoords = IntVec2(38, 30);
	spruce.m_isOpaque = true;
	spruce.m_isSolid = true;
	spruce.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(spruce);

	BlockDef leaf;
	leaf.m_name = "Leaf";
	leaf.m_topTexCoords = IntVec2(32, 28);
	leaf.m_sideTexCoords = IntVec2(32, 28);
	leaf.m_bottomTexCoords = IntVec2(32, 28);
	leaf.m_isOpaque = false;
	leaf.m_isSolid = true;
	leaf.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(leaf);

	BlockDef cactus;
	cactus.m_name = "Cactus";
	cactus.m_topTexCoords = IntVec2(39, 27);
	cactus.m_sideTexCoords = IntVec2(37, 27);
	cactus.m_bottomTexCoords = IntVec2(38, 27);
	cactus.m_isOpaque = false;
	cactus.m_isSolid = true;
	cactus.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(cactus);

	BlockDef snowyGrass;
	snowyGrass.m_name = "Snowy Grass";
	snowyGrass.m_topTexCoords = IntVec2(36, 28);
	snowyGrass.m_sideTexCoords = IntVec2(33, 28);
	snowyGrass.m_bottomTexCoords = IntVec2(32, 29);
	snowyGrass.m_isSolid = true;
	snowyGrass.m_isOpaque = true;
	snowyGrass.m_isVisible = true;
	BlockDef::s_blockDefRegistry->RegisterBlockDef(snowyGrass);

	SpriteSheet sheet = SpriteSheet(*g_theRenderer->CreateOrGetTextureFromFile("Data/Images/BasicSprites_64x64.png"), IntVec2(64, 64));

	BlockDef::s_blockDefRegistry->SetSpriteSheet(sheet);

	Block logBlock = BlockDef::s_blockDefRegistry->GetBlockByName("Oak Log");
	Block leafBlock = BlockDef::s_blockDefRegistry->GetBlockByName("Leaf");
	Block cactusBlock = BlockDef::s_blockDefRegistry->GetBlockByName("Cactus");

	BlockTemplate cactusTemplate;
	cactusTemplate.PushEntry({ IntVec3(0,0,0),  cactusBlock });
	cactusTemplate.PushEntry({ IntVec3(0,0,1),  cactusBlock });
	cactusTemplate.PushEntry({ IntVec3(0,0,2),  cactusBlock });
	cactusTemplate.PushEntry({ IntVec3(0,0,3),  cactusBlock });
	cactusTemplate.PushEntry({ IntVec3(0,0,4),  cactusBlock });
	BlockTemplate::s_blockTemplates["Cactus"] = cactusTemplate;

	BlockTemplate tree;
	tree.PushEntry({ IntVec3(0,0,0),  logBlock });
	tree.PushEntry({ IntVec3(0,0,1),  logBlock });
	tree.PushEntry({ IntVec3(0,0,2),  logBlock });
	tree.PushEntry({ IntVec3(0,0,3),  logBlock });
	tree.PushEntry({ IntVec3(0,0,4),  logBlock });

	for (int zz = 0; zz <= 1; ++zz)
	{
		for (int yy = -2; yy <= 2; ++yy)
		{
			for (int xx = -2; xx <= 2; ++xx)
			{
				if ((xx == -2 || xx == 2) && (yy == -2 || yy == 2))
				{
					continue;
				}
				IntVec3 offset = IntVec3(xx, yy, 4 + zz);
				tree.PushEntry({ offset, leafBlock });
			}
		}
	}

	tree.PushEntry({ IntVec3(0, 0, 6), leafBlock });
	tree.PushEntry({ IntVec3(0, -1, 6), leafBlock });
	tree.PushEntry({ IntVec3(0, 1, 6), leafBlock });
	tree.PushEntry({ IntVec3(-1, 0, 6), leafBlock });
	tree.PushEntry({ IntVec3(1, 0, 6), leafBlock });
	tree.PushEntry({ IntVec3(-1, -1, 6), leafBlock });
	tree.PushEntry({ IntVec3(-1, 1, 6), leafBlock });
	tree.PushEntry({ IntVec3(1, -1, 6), leafBlock });
	tree.PushEntry({ IntVec3(1, 1, 6), leafBlock });

	BlockTemplate::s_blockTemplates["Oak Tree"] = tree;

	logBlock = BlockDef::s_blockDefRegistry->GetBlockByName("Spruce Log");

	BlockTemplate spruceTree;
	spruceTree.PushEntry({ IntVec3(0,0,0),  logBlock });
	spruceTree.PushEntry({ IntVec3(0,0,1),  logBlock });
	spruceTree.PushEntry({ IntVec3(0,0,2),  logBlock });
	spruceTree.PushEntry({ IntVec3(0,0,3),  logBlock });
	spruceTree.PushEntry({ IntVec3(0,0,4),  logBlock });

	for (int zz = 0; zz <= 1; ++zz)
	{
		for (int yy = -2; yy <= 2; ++yy)
		{
			for (int xx = -2; xx <= 2; ++xx)
			{
				if ((xx == -2 || xx == 2) && (yy == -2 || yy == 2))
				{
					continue;
				}
				IntVec3 offset = IntVec3(xx, yy, 4 + zz);
				spruceTree.PushEntry({ offset, leafBlock });
			}
		}
	}

	spruceTree.PushEntry({ IntVec3(0, 0, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(0, -1, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(0, 1, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(-1, 0, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(1, 0, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(-1, -1, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(-1, 1, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(1, -1, 6), leafBlock });
	spruceTree.PushEntry({ IntVec3(1, 1, 6), leafBlock });

	BlockTemplate::s_blockTemplates["Spruce Tree"] = spruceTree;
 }

 void LightingJob::Execute()
 {
	 m_chunk->ProcessDirtyLighting();
 }
