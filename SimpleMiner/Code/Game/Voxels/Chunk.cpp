#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/ThirdParty/Squirrel/SmoothNoise.hpp"
#include "Engine/ThirdParty/Squirrel/RawNoise.hpp"

#include "Game/Voxels/ChunkNoiseMap.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Voxels/Chunk.hpp"
#include "Game/Voxels/VoxelCommon.hpp"
#include "Game/Voxels/BlockDef.hpp"
#include "Game/Voxels/VoxelWorld.hpp"
#include "Game/Voxels/BlockIterator.hpp"
#include "Game/Voxels/BlockTemplate.hpp"
#include "Game/Voxels/VoxelCave.hpp"

#include <iostream>
#include <filesystem>
#include <sstream>

void ChunkMeshBuildJob::Execute()
{
	m_chunk->GenerateMesh();
}

void ChunkLoadJob::Execute()
{
	std::vector<uint8_t> data;

	if (FileReadToBuffer(data, m_chunk->GetFilePath()))
	{
		if (!m_chunk->LoadFromData(data))
		{
			m_chunk->StartGenerationJob();
		}
	}
}


void ChunkGenerationJob::Execute()
{
	m_chunk->GenerateChunkData();
}

uint8_t GetChunkMaxSkyLight(int currentLight, Block* block)
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

uint8_t GetChunkMaxLight(int currentLight, Block* block)
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

void Chunk::ProcessDirtyLighting()
{
	bool processedLighting = false;
	while (!processedLighting)
	{
		for (BlockIterator itr : m_dirtyLightSwapBuffer[m_dirtyLightSwapIndex])
		{
			Block* block = itr.GetBlock();

			if (block == nullptr)
			{
				//This block is invalid no point to check anything else
				return;
			}

			block->SetLightingDirty(false);
			BlockDef def = BlockDef::s_blockDefRegistry->GetBlockDefByType(block->m_type);

			uint8_t skyLight = block->GetSkyLight();
			uint8_t blockLight = block->GetBlockLight();
			uint8_t minLightLevel = def.m_lightLevel;
			uint8_t minSkyLightLevel = block->GetIsSky() && !def.m_isOpaque ? 15 : 0;

			BlockIterator eastItr = itr.GetEastNeighbor();
			BlockIterator westItr = itr.GetWestNeighbor();
			BlockIterator northItr = itr.GetNorthNeighbor();
			BlockIterator southItr = itr.GetSouthNeighbor();
			BlockIterator skywardItr = itr.GetSkywardNeighbor();
			BlockIterator groundwardItr = itr.GetGroundwardNeighbor();

			Block* east = eastItr.GetBlock();
			Block* west = westItr.GetBlock();
			Block* north = northItr.GetBlock();
			Block* south = southItr.GetBlock();
			Block* skyward = skywardItr.GetBlock();
			Block* groundward = groundwardItr.GetBlock();

			minLightLevel = GetChunkMaxLight(minLightLevel, east);
			minLightLevel = GetChunkMaxLight(minLightLevel, west);
			minLightLevel = GetChunkMaxLight(minLightLevel, north);
			minLightLevel = GetChunkMaxLight(minLightLevel, south);
			minLightLevel = GetChunkMaxLight(minLightLevel, skyward);
			minLightLevel = GetChunkMaxLight(minLightLevel, groundward);

			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, east);
			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, west);
			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, north);
			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, south);
			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, skyward);
			minSkyLightLevel = GetChunkMaxSkyLight(minSkyLightLevel, groundward);

			if (def.m_isOpaque && def.m_lightLevel == 0)
			{
				minLightLevel = 0;
				minSkyLightLevel = 0;
			}

			if (blockLight != minLightLevel || skyLight != minSkyLightLevel)
			{
				itr.m_chunk->m_dirtyMesh = true;
				block->SetBlockLight(minLightLevel);
				block->SetSkyLight(minSkyLightLevel);
				m_voxelWorld->MarkDirtyLighting(eastItr);
				m_voxelWorld->MarkDirtyLighting(westItr);
				m_voxelWorld->MarkDirtyLighting(northItr);
				m_voxelWorld->MarkDirtyLighting(southItr);
				m_voxelWorld->MarkDirtyLighting(skywardItr);
				m_voxelWorld->MarkDirtyLighting(groundwardItr);
			}
		}

		SwapLightBuffers();

		if (m_dirtyLightSwapBuffer[m_dirtyLightSwapIndex].size() == 0)
		{
			processedLighting = true;
		}
	}
}

void Chunk::PushLightToBackBuffer(BlockIterator const& iter)
{
	std::lock_guard<std::mutex> lock(m_lightBackBufferMutex);
	size_t index = m_dirtyLightSwapIndex == 0 ? 1 : 0;
	m_dirtyLightSwapBuffer[index].push_back(iter);

}

void Chunk::SwapLightBuffers()
{
	m_dirtyLightSwapBuffer[m_dirtyLightSwapIndex].clear();
	m_dirtyLightSwapIndex = m_dirtyLightSwapIndex == 0 ? 1 : 0;
}

Chunk::Chunk(VoxelWorld* world, IntVec2 coords, uint32_t seed)
	: m_voxelWorld(world)
	, m_chunkCoords(coords)
	, m_seed(seed)
{
	m_bounds =
		AABB3(0,0,0,CHUNK_X_SIZE,CHUNK_Y_SIZE,CHUNK_Z_SIZE);
	m_bounds.Translate(Vec3(coords.x * ((int)CHUNK_X_SIZE), coords.y * ((int)CHUNK_Y_SIZE), 0.f));
}

Chunk::~Chunk()
{
	delete m_vbo;
	m_vbo = nullptr;
	delete[] m_blocks;
	m_blocks = nullptr;
}

void Chunk::Startup()
{
}

void Chunk::Activate()
{
	m_active = true;
}

void Chunk::Deactivate()
{
	m_active = false;
	RemoveNeighborNegX();
	RemoveNeighborNegY();
	RemoveNeighborPosX();
	RemoveNeighborPosY();

	m_vertices.clear();
	delete m_vbo;
	m_vbo = nullptr;
}

bool Chunk::IsActive() const
{
	return m_active;
}

void Chunk::StartMeshBuildJob()
{
	if (m_meshBuildJob == nullptr)
	{
		m_state = ChunkState::BUILDING_MESH;
		m_dirtyMesh    = false;
		m_meshBuildJob = new ChunkMeshBuildJob();
		m_meshBuildJob->m_chunk = this;
		g_theJobSystem->SubmitJob(m_meshBuildJob);
	}

}

void Chunk::StartLoadJob()
{
	if (m_loadJob == nullptr)
	{
		m_loadJob = new ChunkLoadJob();
		m_loadJob->m_chunk = this;
		m_voxelWorld->m_ioJobSystem.SubmitJob(m_loadJob);
	}
}

void Chunk::StartGenerationJob()
{
	if (m_generationJob == nullptr)
	{
		m_generationJob = new ChunkGenerationJob();
		m_generationJob->m_chunk = this;
		g_theJobSystem->SubmitJob(m_generationJob);
	}
}

void Chunk::GenerateChunkData()
{
	m_blocks = new Block[CHUNK_TOTAL_VOXELS];
	std::vector<VoxelCave> caves;

	constexpr int CAVE_RADIUS = 200;
	constexpr int SEARCH_RANGE = CAVE_RADIUS / 16;
	for (int chunkX = -SEARCH_RANGE + m_chunkCoords.x; chunkX < SEARCH_RANGE + m_chunkCoords.x; ++chunkX)
	{
		for (int chunkY = -SEARCH_RANGE + m_chunkCoords.y; chunkY < SEARCH_RANGE + m_chunkCoords.y; ++chunkY)
		{
			// 1 in 64 chance to generate a cave
			if (Get2dNoiseUint(chunkX, chunkY, m_seed + 9) < (~0ui32) / 64)
			{
				IntVec3 globalBlockCoords = IntVec3(chunkX * ((int)CHUNK_X_SIZE), chunkY * ((int)CHUNK_Y_SIZE), CHUNK_Z_SIZE / 4);

				VoxelCave cave(globalBlockCoords, 7.f, CAVE_RADIUS, m_seed + 10);
				if (cave.IsABB3Inside(m_bounds))
				{
					cave.Generate();
					if (cave.IsABB3Inside(m_bounds))
					{
						caves.push_back(cave);
					}
				}
			}
		}
	}

	Block const& dirt = BlockDef::s_blockDefRegistry->GetBlockByName("Dirt");
	Block const& grass = BlockDef::s_blockDefRegistry->GetBlockByName("Grass");
	Block const& stone = BlockDef::s_blockDefRegistry->GetBlockByName("Stone");
	Block const& coal = BlockDef::s_blockDefRegistry->GetBlockByName("Coal Ore");
	Block const& iron = BlockDef::s_blockDefRegistry->GetBlockByName("Iron Ore");
	Block const& gold = BlockDef::s_blockDefRegistry->GetBlockByName("Gold Ore");
	Block const& diamond = BlockDef::s_blockDefRegistry->GetBlockByName("Diamond Ore");
	Block const& water = BlockDef::s_blockDefRegistry->GetBlockByName("Water");
	Block const& sand = BlockDef::s_blockDefRegistry->GetBlockByName("Sand");
	Block const& ice = BlockDef::s_blockDefRegistry->GetBlockByName("Ice");
	Block const& snowyGrass = BlockDef::s_blockDefRegistry->GetBlockByName("Snowy Grass");

	BlockTemplate const& spruceTree = BlockTemplate::s_blockTemplates.at("Spruce Tree");
	BlockTemplate const& oakTree = BlockTemplate::s_blockTemplates.at("Oak Tree");
	BlockTemplate const& cactus = BlockTemplate::s_blockTemplates.at("Cactus");

	constexpr int PADDING = 20;
	constexpr int OCEAN_HEIGHT = CHUNK_Z_SIZE / 2;
	constexpr int OCEAN_MAX_DEPTH = CHUNK_Z_SIZE / 4;

	ChunkNoiseMap<int>   terrainHeightMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<int>   stoneHeightMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> humidityMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> temperatureMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> hillinessMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> oceannessMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> forestnessMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> domainWarpMapX(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> domainWarpMapY(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);
	ChunkNoiseMap<float> cavinessMap(CHUNK_X_SIZE, CHUNK_Y_SIZE, PADDING);

	for (int yy = -PADDING; yy < ((int)CHUNK_Y_SIZE) + PADDING; yy++)
	{
		for (int xx = -PADDING; xx < ((int)CHUNK_X_SIZE) + PADDING; xx++)
		{
			int globalX = m_chunkCoords.x * CHUNK_X_SIZE + xx;
			int globalY = m_chunkCoords.y * CHUNK_Y_SIZE + yy;

			float gX = static_cast<float>(globalX);
			float gY = static_cast<float>(globalY);

			float terrainNoise     = Compute2dPerlinNoise(gX, gY,  250.f, 5, 0.5f, 2.f, true, m_seed);
			float hillinessNoise   = 0.5f * Compute2dPerlinNoise(gX, gY, 500.f, 7, 0.5f, 2.f, true, m_seed + 1) + 0.5f;
			float oceannessNoise   = 0.5f * Compute2dPerlinNoise(gX, gY, 1500.f, 7, 0.5f, 2.f, true, m_seed + 2) + 0.5f;
			float temperatureNoise = 0.5f * Compute2dPerlinNoise(gX, gY, 1000.f, 6, 0.5f, 2.f, true, m_seed + 3) + 0.5f;
			float humidityNoise	   = 0.5f * Compute2dPerlinNoise(gX, gY, 1000.f, 6, 0.5f, 2.f, true, m_seed + 4) + 0.5f;
			float forestnessNoise  = 0.5f * Compute2dPerlinNoise(gX, gY,  100.f, 6, 0.5f, 2.f, true, m_seed + 5) + 0.5f;
			float cavinessNoise    = 0.5f * Compute2dPerlinNoise(gX, gY, 250.f, 3, 0.5f, 2.f, true, m_seed + 6) + 0.5f;
			//float domainWarpX = SmoothStep5(Compute2dPerlinNoise(gX, gY, 500.f, 6, 0.5f, 2.f, true, m_seed + 6), 0.f, 1.f);
			//float domainWarpY = SmoothStep5(Compute2dPerlinNoise(gX, gY, 500.f, 6, 0.5f, 2.f, true, m_seed + 7), 0.f, 1.f);
			float terrainHeightFloat = (60.f * fabsf(terrainNoise));

			float hilliness = RangeMap(SmoothStep5(0.f, 1.f, hillinessNoise), 0.f, 1.f, 0.8f, 1.2f);
			float oceanness = SmoothStep5(0.f, 1.f, oceannessNoise);

			oceannessMap.Set(xx, yy, oceanness);
			temperatureMap.Set(xx, yy, temperatureNoise);
			hillinessMap.Set(xx, yy, hilliness);
			humidityMap.Set(xx, yy, humidityNoise);
			forestnessMap.Set(xx, yy, forestnessNoise);
			cavinessMap.Set(xx, yy, cavinessNoise);
			terrainHeightFloat = terrainHeightFloat + static_cast<float>(OCEAN_HEIGHT);
			if (oceanness > 0.5f)
			{
				terrainHeightFloat = RangeMapClamped(oceanness, 0.5f, 0.75f, terrainHeightFloat, static_cast<float>(OCEAN_MAX_DEPTH));
			}

			terrainHeightFloat *= hilliness;

			terrainHeightMap.Set(xx, yy, static_cast<int>(terrainHeightFloat));

			int stoneHeight = static_cast<int>(terrainHeightFloat) - g_RNG.RollRandomIntInRange(3, 4);
			stoneHeightMap.Set(xx, yy, stoneHeight);

			//domainWarpMapX.Set(xx, yy, domainWarpX);
			//domainWarpMapY.Set(xx, yy, domainWarpY);
		}
	}

	for (int i = 0; i < CHUNK_TOTAL_VOXELS; i++)
	{
		BlockIterator blockItr = BlockIterator(this, i);

		int localX = GetLocalBlockXFromIndex(i);
		int localY = GetLocalBlockYFromIndex(i);
		int localZ = GetLocalBlockZFromIndex(i);

		int globalX = m_chunkCoords.x * CHUNK_X_SIZE + localX;
		int globalY = m_chunkCoords.y * CHUNK_Y_SIZE + localY;

		int globalZ = localZ;
		
		//float domainWarpScaleX = domainWarpMapX.Get(localX, localY);
		//float domainWarpScaleY = domainWarpMapY.Get(localX, localY);
		//
		//float domainWarpX = 20.f * domainWarpScaleX * fabsf(Compute3dFractalNoise(globalX * 5.f, globalY * 5.f, localZ * 10.f, 250.f, 5, 0.5f, 2.f, true, m_seed + 6));;
		//float domainWarpY = 20.f * domainWarpScaleY * fabsf(Compute3dFractalNoise(globalX * 5.f, globalY * 5.f, localZ * 10.f, 250.f, 5, 0.5f, 2.f, true, m_seed + 7));

		int terrainHeight = terrainHeightMap.Get(localX,localY);
		int stoneHeight = stoneHeightMap.Get(localX, localY);

		float temperature = temperatureMap.Get(localX, localY);
		float humidity = humidityMap.Get(localX, localY);

		Block surfaceBlock;
		Block subSurfaceBlock;

		if (humidity > 0.4f)
		{
			surfaceBlock = grass;
			subSurfaceBlock = dirt;
			if (terrainHeight == OCEAN_HEIGHT && humidity < 0.65f)
			{
				surfaceBlock = sand;
			}
		}
		else
		{
			surfaceBlock = sand;
			subSurfaceBlock = sand;
		}

		if (temperature < 0.4f)
		{
			surfaceBlock = snowyGrass;
		}


		if (globalZ <= terrainHeight)
		{
			float caviness = cavinessMap.Get(localX, localY);
			float gradientThreshold = RangeMapClamped(globalZ, static_cast<float>(terrainHeight), static_cast<float>(terrainHeight - 20), 0.5f, 0.f);
			float spacialness = caviness * (0.5f * Compute3dFractalNoise(globalX, globalY, globalZ, 100.f, 5, 0.5f, 2.f, true, m_seed + 8) + 0.5f);
			float worminess = caviness * fabsf(Compute3dFractalNoise(globalX, globalY, globalZ, 100.f, 5, 0.5f, 2.f, true, m_seed + 9));
			if (spacialness > 0.5f + gradientThreshold || worminess - 0.9f - (gradientThreshold * 0.2f) > 0.f)
			{
				goto NextBlockPos;
			}

			for (VoxelCave cave : caves)
			{
				if (cave.IsBlockInside(IntVec3(globalX, globalY, globalZ)))
				{
					goto NextBlockPos;
				}
			}
		}

		if (globalZ < stoneHeight)
		{
			m_blocks[i] = stone;
			if (g_RNG.RollRandomFloatZeroToOne() < 0.0001f)
			{
				m_blocks[i] = diamond;
			}
			else if (g_RNG.RollRandomFloatZeroToOne() < 0.0005f)
			{
				m_blocks[i] = gold;
			}
			else if (g_RNG.RollRandomFloatZeroToOne() < 0.002f)
			{
				m_blocks[i] = iron;
			}
			else if (g_RNG.RollRandomFloatZeroToOne() < 0.005f)
			{
				m_blocks[i] = coal;
			}
			continue;
		}
		else if (globalZ == terrainHeight)
		{
			m_blocks[i] = surfaceBlock;
			continue;
		}
		else if (globalZ < terrainHeight)
		{
			m_blocks[i] = subSurfaceBlock;
			continue;
		}
		else if (globalZ <= OCEAN_HEIGHT)
		{
			if (globalZ == OCEAN_HEIGHT && temperature < 0.4f)
			{
				m_blocks[i] = ice;
			}
			else
			{
				m_blocks[i] = water;
			}
			continue;
		}
	NextBlockPos:;
	}

	//Structure generation pass
	for (int localZ = -PADDING / 2; localZ < ((int)CHUNK_Z_SIZE) + (PADDING / 2); ++localZ)
	{
		for (int localY = -PADDING / 2; localY < ((int)CHUNK_Y_SIZE) + (PADDING / 2); ++localY)
		{
			for (int localX = -PADDING / 2; localX < ((int)CHUNK_X_SIZE) + (PADDING / 2); ++localX)
			{
				int index = GetIndexFromLocalBlockCoords(IntVec3(localX, localY, localZ));
				float terrainHeight = terrainHeightMap.Get(localX, localY);
				float humidity = humidityMap.Get(localX, localY);
				float temperature = temperatureMap.Get(localX, localY);

				if (terrainHeight >= OCEAN_HEIGHT && localZ == terrainHeight)
				{
					float forestness = forestnessMap.Get(localX, localY);
					if (forestness > 0.6f)
					{
						bool generateTree = true;

						for (int yy = -PADDING / 2; yy <= PADDING / 2; ++yy)
						{
							for (int xx = -PADDING / 2; xx <= PADDING / 2; ++xx)
							{
								float contendingMaxima = forestnessMap.Get(localX + xx, localY + yy);
								if (contendingMaxima > forestness)
								{
									generateTree = false;
								}
							}
						}

						if (generateTree)
						{
							if (temperature < 0.4f)
							{
								spruceTree.GenerateIntoChunk(this, IntVec3(localX, localY, localZ + 1));
							}
							else
							{
								if (humidity < 0.4)
								{
									cactus.GenerateIntoChunk(this, IntVec3(localX, localY, localZ + 1));
								}
								else
								{
									oakTree.GenerateIntoChunk(this, IntVec3(localX, localY, localZ + 1));
							}
							}
						}
					}
				}
			}
		}
	}
}

void Chunk::DirtyLightsAfterGeneration()
{
	for (int y = 0; y < CHUNK_Y_SIZE; ++y)
	{
		for (int x = 0; x < CHUNK_X_SIZE; ++x)
		{
			IntVec3 localPos = IntVec3(x, y, CHUNK_Z_SIZE - 1);
			int index = GetIndexFromLocalBlockCoords(localPos);
			BlockIterator itr = BlockIterator(this, index);
			BlockIterator itrSky = itr;
			Block* block = itrSky.GetBlock();
			do
			{
				block->SetIsSky(true);
				m_voxelWorld->MarkDirtyLighting(itrSky);
				itrSky = itrSky.GetGroundwardNeighbor();
				block = itrSky.GetBlock();
				if (block == nullptr)
				{
					goto NextPos;
				}
			} while (!itrSky.GetBlock()->GetBlockDef().m_isOpaque);

		NextPos:;
		}
	}

	for (int blockIndex = 0; blockIndex < CHUNK_TOTAL_VOXELS; blockIndex++)
	{
		Block block = m_blocks[blockIndex];
		if (block.m_type == 0)
		{
			BlockIterator blockItr = BlockIterator(this, blockIndex);

			IntVec3 localBlockCoords = GetLocalBlockCoordsFromIndex(blockIndex);
			int localX = localBlockCoords.x;
			int localY = localBlockCoords.y;

			//Dirty if on neighbor chunk boundary
			if (localX == 0)
			{
				m_voxelWorld->MarkDirtyLighting(blockItr);
			}
			if (localX == CHUNK_X_SIZE - 1)
			{
				m_voxelWorld->MarkDirtyLighting(blockItr);
			}

			if (localY == 0)
			{
				m_voxelWorld->MarkDirtyLighting(blockItr);
			}
			if (localY == CHUNK_Y_SIZE - 1)
			{
				m_voxelWorld->MarkDirtyLighting(blockItr);
			}
		}
		else
		{
			BlockIterator blockItr = BlockIterator(this, blockIndex);

			BlockDef def = BlockDef::s_blockDefRegistry->GetBlockDefByType(block.m_type);
			if (def.m_lightLevel > 0)
			{
				m_voxelWorld->MarkDirtyLighting(blockItr);
			}
		}
	}
}

void Chunk::GenerateMesh()
{
	m_vertices.clear();

	SpriteSheet const& spriteSheet = BlockDef::s_blockDefRegistry->GetSpriteSheet();

	if (m_blocks == nullptr)
	{
		ERROR_AND_DIE("Generated mesh on chunk without blocks");
	}

	for (int i = 0; i < CHUNK_TOTAL_VOXELS; i++)
	{
		BlockDef const& blockDef = BlockDef::s_blockDefRegistry->GetBlockDefByType(m_blocks[i].m_type);
		if (blockDef.m_isVisible == false)
		{
			continue;
		}

		AABB3 blockBounds = AABB3(0, 0, 0, 1, 1, 1);
		IntVec3 intCoords = GetLocalBlockCoordsFromIndex(i);
		Vec3 coords = Vec3((float)intCoords.x, (float)intCoords.y, (float)intCoords.z);
		blockBounds.Translate(Vec3(coords.x, coords.y, coords.z));

		SpriteDefinition topDef    = spriteSheet.GetSpriteDefinition(blockDef.m_topTexCoords);
		SpriteDefinition sideDef   = spriteSheet.GetSpriteDefinition(blockDef.m_sideTexCoords);
		SpriteDefinition bottomDef = spriteSheet.GetSpriteDefinition(blockDef.m_bottomTexCoords);

		BlockIterator myBlockIterator = BlockIterator(this, i);
		BlockIterator eastItr		= myBlockIterator.GetEastNeighbor();
		BlockIterator westItr		= myBlockIterator.GetWestNeighbor();
		BlockIterator northItr		= myBlockIterator.GetNorthNeighbor();
		BlockIterator southItr		= myBlockIterator.GetSouthNeighbor();
		BlockIterator skywardItr	= myBlockIterator.GetSkywardNeighbor();
		BlockIterator groundwardItr = myBlockIterator.GetGroundwardNeighbor();

		Block otherBlock = GetBlockFromLocalBlockPos(intCoords + IntVec3(0, 0, 1));
		Block* blockPtr = nullptr;

		if (otherBlock.m_type == 0 || !g_theGame->m_config.m_hiddenSurfaceRemoval)
		{
			Block* up = myBlockIterator.GetSkywardNeighbor().GetBlock();
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			if (up)
			{
				lightInfluence = static_cast<uint8_t>((up->GetBlockLight() / 16.f) * 255);
 				skyLightInfluence = static_cast<uint8_t>((up->GetSkyLight() / 16.f) * 255);
			}

			//TOP
 			AddVertsForQuad3D(
				m_vertices, Vec3(1, 0, 1) + coords, Vec3(0, 0, 1) + coords, Vec3(0, 1, 1) + coords, Vec3(1, 1, 1) + coords, Rgba8(skyLightInfluence,lightInfluence,255), topDef.GetUVBounds()
			);
		}

		blockPtr = westItr.GetBlock();
		if (blockPtr)
		{
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			lightInfluence = static_cast<uint8_t>((blockPtr->GetBlockLight() / 16.f) * 255);
			skyLightInfluence = static_cast<uint8_t>((blockPtr->GetSkyLight() / 16.f) * 255);
			BlockDef const& blockDef = blockPtr->GetBlockDef();
			//SIDES
			if (!blockDef.m_isOpaque || !g_theGame->m_config.m_hiddenSurfaceRemoval)
			{
				AddVertsForQuad3D(m_vertices, Vec3(1, 0, 0) + coords, Vec3(0, 0, 0) + coords, Vec3(0, 0, 1) + coords, Vec3(1, 0, 1) + coords, Rgba8(skyLightInfluence, lightInfluence, 230), sideDef.GetUVBounds());
			}
		}

		blockPtr = eastItr.GetBlock();
		if (blockPtr)
		{
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			lightInfluence = static_cast<uint8_t>((blockPtr->GetBlockLight() / 16.f) * 255);
			skyLightInfluence = static_cast<uint8_t>((blockPtr->GetSkyLight() / 16.f) * 255);
			BlockDef const& blockDef = blockPtr->GetBlockDef();
			if (!blockDef.m_isOpaque || !g_theGame->m_config.m_hiddenSurfaceRemoval)
			{
				AddVertsForQuad3D(m_vertices, Vec3(0, 1, 0) + coords, Vec3(1, 1, 0) + coords, Vec3(1, 1, 1) + coords, Vec3(0, 1, 1) + coords, Rgba8(skyLightInfluence, lightInfluence, 200), sideDef.GetUVBounds());
			}
		}

		blockPtr = southItr.GetBlock();
		if (blockPtr)
		{
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			lightInfluence = static_cast<uint8_t>((blockPtr->GetBlockLight() / 16.f) * 255);
			skyLightInfluence = static_cast<uint8_t>((blockPtr->GetSkyLight() / 16.f) * 255);
			BlockDef const& blockDef = blockPtr->GetBlockDef();
			if (!blockDef.m_isOpaque || !g_theGame->m_config.m_hiddenSurfaceRemoval)
			{
				AddVertsForQuad3D(m_vertices, Vec3(0, 0, 0) + coords, Vec3(0, 1, 0) + coords, Vec3(0, 1, 1) + coords, Vec3(0, 0, 1) + coords, Rgba8(skyLightInfluence, lightInfluence, 200), sideDef.GetUVBounds());
			}
		}

		blockPtr = northItr.GetBlock();
		if (blockPtr)
		{
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			lightInfluence = static_cast<uint8_t>((blockPtr->GetBlockLight() / 16.f) * 255);
			skyLightInfluence = static_cast<uint8_t>((blockPtr->GetSkyLight() / 16.f) * 255);
			BlockDef const& blockDef = blockPtr->GetBlockDef();
			if (!blockDef.m_isOpaque || !g_theGame->m_config.m_hiddenSurfaceRemoval)
			{
				AddVertsForQuad3D(m_vertices, Vec3(1, 1, 0) + coords, Vec3(1, 0, 0) + coords, Vec3(1, 0, 1) + coords, Vec3(1, 1, 1) + coords, Rgba8(skyLightInfluence, lightInfluence, 230), sideDef.GetUVBounds());
			}
		}

		//BOTTOM
		otherBlock = GetBlockFromLocalBlockPos(intCoords + IntVec3(0, 0, -1));
		if (otherBlock.m_type == 0 || intCoords.z == 0 || !g_theGame->m_config.m_hiddenSurfaceRemoval)
		{
			Block* down = myBlockIterator.GetGroundwardNeighbor().GetBlock();
			uint8_t lightInfluence = 0;
			uint8_t skyLightInfluence = 0;
			if (down)
			{
				lightInfluence = static_cast<uint8_t>((down->GetBlockLight() / 16.f) * 255);
				skyLightInfluence = static_cast<uint8_t>((down->GetSkyLight() / 16.f) * 255);
			}

			AddVertsForQuad3D(m_vertices, Vec3(0, 0, 0) + coords, Vec3(1, 0, 0) + coords, Vec3(1, 1, 0) + coords, Vec3(0, 1, 0) + coords, Rgba8(skyLightInfluence, lightInfluence, 200), bottomDef.GetUVBounds());
		}
	}
}

Block Chunk::GetBlockFromLocalBlockPos(IntVec3 blockPos)
{
	int index = GetIndexFromLocalBlockCoords(blockPos);

	if (index < 0 || index > CHUNK_TOTAL_VOXELS - 1)
	{
		Block block;
		block.m_type = 0;
		return block;
	}

	return m_blocks[index];
}

void Chunk::SetBlock(IntVec3 localBlockCoords, Block block)
{
	m_dirtyData = true;
	m_dirtyMesh = true;

	int index = GetIndexFromLocalBlockCoords(localBlockCoords);

	BlockIterator itr = BlockIterator(this, index);

	BlockDef def = block.GetBlockDef();
	BlockIterator up = itr.GetSkywardNeighbor();
	BlockIterator down = itr.GetGroundwardNeighbor();

	m_voxelWorld->MarkDirtyLighting(up);
	m_voxelWorld->MarkDirtyLighting(down);
	m_voxelWorld->MarkDirtyLighting(itr);
	m_voxelWorld->MarkDirtyLighting(itr.GetNorthNeighbor());
	m_voxelWorld->MarkDirtyLighting(itr.GetSouthNeighbor());
	m_voxelWorld->MarkDirtyLighting(itr.GetWestNeighbor());
	m_voxelWorld->MarkDirtyLighting(itr.GetEastNeighbor());

	if (Block* currentBlock = itr.GetBlock())
	{
		if (currentBlock->GetIsSky() && def.m_isOpaque)
		{
			Block* currentBlock = itr.GetBlock();
			block.SetIsSky(false);
			BlockIterator itrSky = itr;
			Block* blockPtr = itrSky.GetBlock();
			do
			{
				blockPtr->SetIsSky(false);
				m_voxelWorld->MarkDirtyLighting(itrSky);
				itrSky = itrSky.GetGroundwardNeighbor();
				blockPtr = itrSky.GetBlock();
				if (blockPtr == nullptr)
				{
					break;
				}
			} while (!itrSky.GetBlock()->GetBlockDef().m_isOpaque);
		}
	}

	if (Block* upBlock = up.GetBlock())
	{
		if (upBlock->GetIsSky() && !def.m_isOpaque)
		{
			Block* currentBlock = itr.GetBlock();
			block.SetIsSky(true);
			BlockIterator itrSky = itr;
			Block* blockPtr = itrSky.GetBlock();
			do
			{
				blockPtr->SetIsSky(true);
				m_voxelWorld->MarkDirtyLighting(itrSky);
				itrSky = itrSky.GetGroundwardNeighbor();
				blockPtr = itrSky.GetBlock();
				if (blockPtr == nullptr)
				{
					break;
				}
			} while (!itrSky.GetBlock()->GetBlockDef().m_isOpaque);
		}
	}

	m_blocks[index] = block;
}

Block* Chunk::GetBlock(IntVec3 localBlockCoords) const
{
	int index = GetIndexFromLocalBlockCoords(localBlockCoords);
	return &m_blocks[index];
}

void Chunk::DebugRender()
{
	//std::vector<Vertex_PCU> debugVerts;
	//AddVertsForAABB3D(debugVerts, m_bounds, Rgba8::WHITE);
	//for (int i = 0; i < CHUNK_TOTAL_VOXELS; ++i)
	//{
	//	IntVec3 pos = GetLocalBlockCoordsFromIndex(i);
	//	AABB3 bounds = AABB3(Vec3(pos.x,pos.y,pos.z),Vec3(pos.x + 1, pos.y + 1, pos.z + 1));
	//	if (m_blocks[i].m_light > 0)
	//	{
	//		AddVertsForAABB3D(debugVerts, bounds, Rgba8::WHITE);
	//	}
	//}
	//g_theRenderer->BindTexture(nullptr);
	//g_theRenderer->SetModelConstants(Mat44::CreateTranslation3D(Vec3(m_chunkCoords.x * 16.f, m_chunkCoords.y * 16.f, 0.f)));
	//g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	//g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	//g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
	//g_theRenderer->DrawVertexArray((int)debugVerts.size(), debugVerts.data());
	//
}

void Chunk::Update()
{
	if (m_active)
	{
		switch (m_state)
		{
			case ChunkState::DISABLED:
			{
				if (m_saveJob || m_generationJob || m_loadJob)
				{
					if (m_saveJob)
					{
						if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_saveJob))
						{
							delete m_saveJob;
							m_saveJob = nullptr;
						}
					}
					if (m_generationJob)
					{
						if (g_theJobSystem->ClaimJobIfComplete(m_generationJob))
						{
							delete m_generationJob;
							m_generationJob = nullptr;
						}
					}
					if (m_loadJob)
					{
						if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_loadJob))
						{
							delete m_loadJob;
							m_loadJob = nullptr;
						}
					}
				}
				else
				{
					StartGenerateOrLoad();
				}
				break;
			}

			case ChunkState::QUEUED_FOR_SAVING:
			case ChunkState::SAVING:
			{
				if (m_saveJob)
				{
					if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_saveJob))
					{
						delete m_saveJob;
						m_saveJob = nullptr;
						StartGenerateOrLoad();
					}
					else
					{
						if (m_saveJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_SAVING;
						}
						else if (m_saveJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::SAVING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Save job was null in chunk in saving state!");
				}
				break;
			}

			case ChunkState::QUEUED_FOR_GENERATION:
			case ChunkState::GENERATING:
			{
				if (m_generationJob)
				{
					if (g_theJobSystem->ClaimJobIfComplete(m_generationJob))
					{
						delete m_generationJob;
						m_generationJob = nullptr;
						DirtyLightsAfterGeneration();
						m_state = ChunkState::ACTIVE;
					}
					else
					{
						if (m_generationJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_GENERATION;
						}
						else if (m_generationJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::GENERATING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Generation job was null in chunk in generating state!");
				}
				break;
			}

			case ChunkState::QUEUED_FOR_LOAD:
			case ChunkState::LOADING:
			{
				if (m_loadJob)
				{
					if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_loadJob))
					{
						delete m_loadJob;
						m_loadJob = nullptr;
						DirtyLightsAfterGeneration();
						m_state = ChunkState::ACTIVE;
					}
					else
					{
						if (m_loadJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_LOAD;
						}
						else if (m_loadJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::LOADING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Load job was null in chunk in loading state!");
				}
				break;
			}
			case ChunkState::COMPLETE:
			case ChunkState::ACTIVE:
			{
					if (
						m_neighorNegX != nullptr &&
						m_neighorPosX != nullptr &&
						m_neighorPosY != nullptr &&
						m_neighorNegY != nullptr
						)
					{
						if (m_neighorNegX->m_neighorNegY != nullptr &&
							m_neighorNegX->m_neighorPosY != nullptr &&
							m_neighorPosX->m_neighorNegY != nullptr &&
							m_neighorPosX->m_neighorPosY != nullptr)
						{
							if (m_dirtyMesh)
							{
								StartMeshBuildJob();
							}
						}
					}
				break;
			}

			case ChunkState::BUILDING_MESH:
			{
				if (g_theJobSystem->ClaimJobIfComplete(m_meshBuildJob))
				{
					delete m_meshBuildJob;
					m_meshBuildJob = nullptr;
					m_state = ChunkState::COMPLETE;
					m_dirtyMesh = false;
					if (m_vertices.size() == 0)
					{
						return;
					}
					delete m_vbo;
					m_vbo = g_theRenderer->CreateVertexBufferFromVertexPCUArray(m_vertices);
					m_vertexCount = (int)m_vertices.size();
				}
				break;
			}
		}
	}
	else
	{
		switch (m_state)
		{
			case ChunkState::BUILDING_MESH:
			{

				if (g_theJobSystem->ClaimJobIfComplete(m_meshBuildJob))
				{
					delete m_meshBuildJob;
					m_meshBuildJob = nullptr;
					m_state = ChunkState::COMPLETE;
					m_dirtyMesh = false;
					if (m_vertices.size() == 0)
					{
						return;
					}
					delete m_vbo;
					m_vbo = g_theRenderer->CreateVertexBufferFromVertexPCUArray(m_vertices);
					m_vertexCount = (int)m_vertices.size();
				}
				break;
			}
			case ChunkState::ACTIVE:
			case ChunkState::COMPLETE:
			{
				if (m_dirtyData)
				{
					ChunkSaveJob* saveJob = new ChunkSaveJob();
					saveJob->m_chunk = this;

					m_voxelWorld->m_ioJobSystem.SubmitJob(saveJob);
					m_state = ChunkState::QUEUED_FOR_SAVING;
					m_saveJob = saveJob;
				}
				else
				{
					m_state = ChunkState::DISABLED;
				}
				break;
			}

			case ChunkState::QUEUED_FOR_SAVING:
			case ChunkState::SAVING:
			{
				if (m_saveJob)
				{
					if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_saveJob))
					{
						delete m_saveJob;
						m_saveJob = nullptr;
						m_state = ChunkState::DISABLED;
						m_dirtyData = false;
					}
					else
					{
						if (m_saveJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_SAVING;
						}
						else if (m_saveJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::SAVING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Save job was null in chunk in saving state!");
				}
				break;
			}

			case ChunkState::QUEUED_FOR_GENERATION:
			case ChunkState::GENERATING:
			{
				if (m_generationJob)
				{
					if (g_theJobSystem->ClaimJobIfComplete(m_generationJob))
					{
						delete m_generationJob;
						m_generationJob = nullptr;
						m_state = ChunkState::DISABLED;
					}
					else
					{
						if (m_generationJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_GENERATION;
						}
						else if (m_generationJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::GENERATING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Generation job was null in chunk in generating state!");
				}
				break;
			}

			case ChunkState::QUEUED_FOR_LOAD:
			case ChunkState::LOADING:
			{
				if (m_loadJob)
				{
					if (m_voxelWorld->m_ioJobSystem.ClaimJobIfComplete(m_loadJob))
					{
						delete m_loadJob;
						m_loadJob = nullptr;
						m_state = ChunkState::DISABLED;
					}
					else
					{
						if (m_loadJob->GetStatus() == JobStatus::WAITING)
						{
							m_state = ChunkState::QUEUED_FOR_LOAD;
						}
						else if (m_loadJob->GetStatus() == JobStatus::CLAIMED)
						{
							m_state = ChunkState::LOADING;
						}
					}
				}
				else
				{
					ERROR_AND_DIE("Load job was null in chunk in loading state!");
				}
				break;
			}

			case ChunkState::DISABLED:
			{
				if (m_blocks)
				{
					if (m_dirtyData)
					{
						ChunkSaveJob* saveJob = new ChunkSaveJob();
						saveJob->m_chunk = this;

						m_voxelWorld->m_ioJobSystem.SubmitJob(saveJob);
						m_state = ChunkState::QUEUED_FOR_SAVING;
						m_saveJob = saveJob;
						break;
					}
					if (m_blocks != nullptr && m_generationJob == nullptr && m_loadJob == nullptr)
					{
						delete[] m_blocks;
						m_blocks = nullptr;
					}
				}
				break;
			}
		
		}
	}
}

void Chunk::Render()
{
	if (m_vbo == nullptr)
	{
		return;
	}

	SpriteSheet const& spriteSheet = BlockDef::s_blockDefRegistry->GetSpriteSheet();
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->BindTexture(&spriteSheet.GetTexture());
	g_theRenderer->SetModelConstants(Mat44::CreateTranslation3D(Vec3(m_chunkCoords.x * 16.f, m_chunkCoords.y * 16.f, 0.f)));
	g_theRenderer->BindShader(g_theRenderer->GetOrCreateShader("Miner"));


	g_theRenderer->DrawVertexBuffer(m_vbo,m_vertexCount,0);
}

bool Chunk::AllNeighborsAreNotProcessingLighting() const
{
	bool value = true;

	if (m_neighorPosX)
	{
		value &= !m_neighorPosX->m_processingLighting;
	}

	if (m_neighorNegX)
	{
		value &= !m_neighorNegX->m_processingLighting;
	}

	if (m_neighorPosY)
	{
		value &= !m_neighorPosY->m_processingLighting;
	}

	if (m_neighorNegY)
	{
		value &= !m_neighorNegY->m_processingLighting;
	}

	return value;
}

void Chunk::AddNeighborPosX(Chunk* chunk)
{
	if (chunk)
	{
		m_neighorPosX = chunk;
		chunk->m_neighorNegX = this;
	}
}

void Chunk::AddNeighborNegX(Chunk* chunk)
{
	if (chunk)
	{
		m_neighorNegX = chunk;
		chunk->m_neighorPosX = this;
	}
}

void Chunk::AddNeighborPosY(Chunk* chunk)
{
	if (chunk)
	{
		m_neighorPosY = chunk;
		chunk->m_neighorNegY = this;
	}
}

void Chunk::AddNeighborNegY(Chunk* chunk)
{
	if (chunk)
	{
		m_neighorNegY = chunk;
		chunk->m_neighorPosY = this;
	}
}

void Chunk::RemoveNeighborPosX()
{
	if (m_neighorPosX)
	{
		m_neighorPosX->m_hasAllNeighbors = false;
		m_neighorPosX->m_neighorNegX = nullptr;
	}
	m_neighorPosX = nullptr;
}

void Chunk::RemoveNeighborNegX()
{
	if (m_neighorNegX)
	{
		m_neighorNegX->m_hasAllNeighbors = false;
		m_neighorNegX->m_neighorPosX = nullptr;
	}
	m_neighorNegX = nullptr;
}

void Chunk::RemoveNeighborPosY()
{
	if (m_neighorPosY)
	{
		m_neighorPosY->m_hasAllNeighbors = false;
		m_neighorPosY->m_neighorNegY = nullptr;
	}
	m_neighorPosY = nullptr;
}

void Chunk::RemoveNeighborNegY()
{
	if (m_neighorNegY)
	{
		m_neighorNegY->m_hasAllNeighbors = false;
		m_neighorNegY->m_neighorPosY = nullptr;
	}
	m_neighorNegY = nullptr;
}

IntVec2 Chunk::GetChunkCoords()
{
	return m_chunkCoords;
}

std::string Chunk::GetFilePath()
{
	std::stringstream ss;
	ss << "Saves/World_" << m_seed << "/Chunk(" << m_chunkCoords.x << ", " << m_chunkCoords.y << ").chunk";
	return ss.str();
}

void Chunk::SaveToFile()
{
	m_dirtyData = false;

	std::vector<uint8_t> data;
	data.push_back('G');
	data.push_back('C');
	data.push_back('H');
	data.push_back('K');
	data.push_back(2);
	data.push_back(CHUNK_X_BITS);
	data.push_back(CHUNK_Y_BITS);
	data.push_back(CHUNK_Z_BITS);

	uint8_t currentTypeCount = 1;
	uint8_t currentType = m_blocks[0].m_type;

	for (int i = 1; i < CHUNK_TOTAL_VOXELS; ++i)
	{
		Block block = m_blocks[i];
		if (currentTypeCount == 255)
		{
			data.push_back(currentType);
			data.push_back(currentTypeCount);
			currentTypeCount = 0;
		}
		else if (currentType == block.m_type)
		{
			++currentTypeCount;
		}
		else
		{
			data.push_back(currentType);
			data.push_back(currentTypeCount);
			currentType = block.m_type;
			currentTypeCount = 1;
		}
	}

	
	FileWriteFromBuffer(data, GetFilePath());
}

bool Chunk::LoadFromData(std::vector<uint8_t> const& data)
{
	if (m_blocks == nullptr)
	{
		m_blocks = new Block[CHUNK_TOTAL_VOXELS];
	}

	bool isValid = data[0] == 'G' && data[1] == 'C' && data[2] == 'H' && data[3] == 'K' &&
		data[4] == 2 && data[5] == CHUNK_X_BITS && data[6] == CHUNK_Y_BITS && data[7] == CHUNK_Z_BITS;

	if (isValid)
	{
		size_t blockIndex = 0;
		for (size_t i = 8; i < data.size(); i += 2)
		{
			Block block;
			block.m_type  = data[i];
			uint8_t count = data[i + 1];
			for (size_t j = 0; j < count; ++j)
			{
				m_blocks[blockIndex] = block;
				++blockIndex;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Chunk::StartGenerateOrLoad()
{
	std::filesystem::path path = GetFilePath();
	if (std::filesystem::exists(path))
	{
		StartLoadJob();
		m_state = ChunkState::QUEUED_FOR_LOAD;
	}
	else
	{
		StartGenerationJob();
		m_state = ChunkState::QUEUED_FOR_GENERATION;
	}
}

void ChunkSaveJob::Execute()
{
	m_chunk->SaveToFile();
}
