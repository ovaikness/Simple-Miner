#pragma once
#include "GameCommon.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Engine/Renderer/BitmapFont.hpp"

#include "Game/Voxels/VoxelWorld.hpp"

#include <map>

class Player;

class GameConfig
{
public:
	int m_chunkActivationRangeBlocks = 500;
	bool m_hiddenSurfaceRemoval = true;
	int m_seed = 0;
};

class Game
{
public:
	int m_selectedBlockType = 1;
	bool m_day = false;
	bool m_debug = false;
	Clock m_gameClock;
	Clock m_voxelClock;
	Timer m_dayTimer;
	Camera const* m_activeCamera = nullptr;
	Camera m_screenCamera;

	VoxelWorld* m_voxelWorld = nullptr;
	Player* m_player = nullptr;
	GameConfig m_config;
	Vec3 m_rayStart;
	Vec3 m_rayFwd;
	float m_rayDist = 10.f;
	RaycastResult3D m_rayResult;
	bool m_rayFree = false;
public:
	~Game();
	Game();
	void		   LoadDefinitions();
	void		   Startup();
	void		   Update();
	void		   Render();

	void		   SetActiveCamera(Camera const& camera);
private:
	void		   HandleInput();
};