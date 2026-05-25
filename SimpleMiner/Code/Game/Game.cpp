#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

#include "Engine/ThirdParty/Squirrel/SmoothNoise.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

#include "Game/Entities/Player.hpp"

Game* g_theGame = nullptr;
RandomNumberGenerator g_RNG;

Game::~Game()
{
	delete m_voxelWorld;
}

Game::Game()
	:m_activeCamera(nullptr)
	, m_voxelClock(m_gameClock)
	, m_dayTimer(600.f, &m_voxelClock)
{
	g_theGame = this;
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
}

void Game::LoadDefinitions()
{
	XmlDocument doc;
	doc.LoadFile("Data/GameConfig.xml");
	XmlElement* root = doc.FirstChildElement("GameConfig");

	m_config.m_chunkActivationRangeBlocks	= ParseXMLAttribute(*root, "chunkActivationRange", m_config.m_chunkActivationRangeBlocks);
	m_config.m_hiddenSurfaceRemoval			= ParseXMLAttribute(*root, "hiddenSurfaceRemoval", m_config.m_hiddenSurfaceRemoval);
	m_config.m_seed	= ParseXMLAttribute(*root, "seed", m_config.m_seed);
}

void Game::Startup()
{
	LoadDefinitions();

	m_voxelWorld = new VoxelWorld(this, static_cast<uint32_t>(m_config.m_seed));
	m_voxelWorld->Startup();

	m_player = new Player(this);
}

void Game::Update()
{
	g_theInput->SetCursorState(true, true);
	HandleInput();
	m_player->Update(m_gameClock.GetDeltaSeconds());
	
	DebugAddWorldBasis(Mat44::IDENTITY, 0.f);

	Mat44 mat;
	mat.SetTranslation3D(m_player->m_camera.GetForward() * 4.f + m_player->m_position);
	mat.SetIBasis3D(Vec3(0.1f, 0.f, 0.f));
	mat.SetJBasis3D(Vec3(0.f, 0.1f, 0.f));
	mat.SetKBasis3D(Vec3(0.f, 0.f, 0.1f));

	DebugAddWorldBasis(mat, 0.f);

	m_voxelWorld->Update(m_player->m_camera.GetPosition());
	if (g_theInput->IsKeyDown('Y'))
	{
		m_voxelClock.SetTimeScale(50.f);
	}
	else
	{
		m_voxelClock.SetTimeScale(1.f);
	}
	if (g_theInput->WasKeyJustPressed('0'))
	{
		m_selectedBlockType = 1;
	}
	if (g_theInput->WasKeyJustPressed('1'))
	{
		m_selectedBlockType = 2;
	}
	if (g_theInput->WasKeyJustPressed('2'))
	{
		m_selectedBlockType = 3;
	}
	if (g_theInput->WasKeyJustPressed('3'))
	{
		m_selectedBlockType = 4;
	}
	if (g_theInput->WasKeyJustPressed('4'))
	{
		m_selectedBlockType = 5;
	}
	if (g_theInput->WasKeyJustPressed('5'))
	{
		m_selectedBlockType = 6;
	}
	if (g_theInput->WasKeyJustPressed('6'))
	{
		m_selectedBlockType = 7;
	}
	if (g_theInput->WasKeyJustPressed('7'))
	{
		m_selectedBlockType = 8;
	}
	if (g_theInput->WasKeyJustPressed('8'))
	{
		m_selectedBlockType = 9;
	}
	if (g_theInput->WasKeyJustPressed('9'))
	{
		m_selectedBlockType = 10;
	}


	if (g_theInput->WasKeyJustPressed('R'))
	{
		m_rayFree = !m_rayFree;
	}

	if (!m_rayFree)
	{
		Vec3 pos = m_player->m_camera.GetPosition();
		Vec3 fwd = m_player->m_camera.GetForward().GetNormalized();

		RaycastResult3D result = m_voxelWorld->RaycastVsBlocks(pos, fwd, m_rayDist);
		m_rayStart = pos;
		m_rayResult = result;
		m_rayFwd = fwd;
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_LMB))
	{
		Block air = BlockDef::s_blockDefRegistry->GetBlockByName("Air");

		if (m_rayResult.m_didImpact)
		{
			m_voxelWorld->SetBlock(m_rayResult.m_tileCoords, air);
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RMB))
	{
		Block air = BlockDef::s_blockDefRegistry->GetBlockByName("Air");

		Block blockToPlace;
		blockToPlace.m_type = m_selectedBlockType;

		IntVec3 offsetCoords = IntVec3(m_rayResult.m_tileCoords.x + m_rayResult.m_impactNormal.x, m_rayResult.m_tileCoords.y + m_rayResult.m_impactNormal.y, m_rayResult.m_tileCoords.z + m_rayResult.m_impactNormal.z);

		if (m_rayResult.m_didImpact)
		{
			m_voxelWorld->SetBlock(offsetCoords, blockToPlace);
		}
	}
}

void Game::Render()
{
	float lightningFrac = RangeMapClamped(Compute1dPerlinNoise(m_dayTimer.GetElapsedTime() * 0.4), 0.6f, 0.9f, 0.f, 1.f);
	float glowFrac = RangeMapClamped(Compute1dPerlinNoise(m_dayTimer.GetElapsedTime() + 10049.f), -1.f, 1.f, 0.8f, 1.f);

	Rgba8 skyColor = Rgba8(200, 230, 255);
	Rgba8 duskColor = Rgba8(20, 20, 40);

	Rgba8 outDoorDay = Rgba8::WHITE;
	Rgba8 outDoorDusk = duskColor;

	Rgba8 lightColor = Rgba8(255, 240, 204);

	Rgba8 lightFinalColor = Rgba8::Mix(Rgba8::BLACK,lightColor,glowFrac);

	while (m_dayTimer.DecrementPeriodIfElapsed())
	{
		m_day = !m_day;
	}

	Rgba8 finalColor;
	Rgba8 outDoor;

	if (m_day)
	{
		finalColor = Rgba8::Mix(duskColor, skyColor, m_dayTimer.GetElapsedFraction());
		outDoor = Rgba8::Mix(outDoorDusk, outDoorDay, m_dayTimer.GetElapsedFraction());
	}
	else
	{
		finalColor = Rgba8::Mix(skyColor, duskColor, m_dayTimer.GetElapsedFraction());
		outDoor = Rgba8::Mix(outDoorDay, outDoorDusk, m_dayTimer.GetElapsedFraction());
	}

	finalColor = Rgba8::Mix(finalColor, Rgba8::WHITE, lightningFrac);
	outDoor = Rgba8::Mix(finalColor, Rgba8::WHITE, lightningFrac);

	g_theRenderer->ClearScreen(finalColor);

	if (m_activeCamera == nullptr)
	{
		return;
	}

	g_theRenderer->BeginCamera(*m_activeCamera);
	DebugRenderWorld(*m_activeCamera);
	g_theRenderer->EndCamera(*m_activeCamera);
	g_theRenderer->BeginCamera(m_screenCamera);
	DebugRenderScreen(m_screenCamera);
	g_theRenderer->EndCamera(m_screenCamera);

	g_theRenderer->BeginCamera(m_player->m_camera);
	g_theRenderer->SetMinerConstants(m_player->m_camera.GetPosition(), lightFinalColor, outDoor, finalColor, m_config.m_chunkActivationRangeBlocks * 0.5, m_config.m_chunkActivationRangeBlocks - 16);
	m_voxelWorld->Render();

	std::vector<Vertex_PCU> vertsForSelector;
	Vec3 coords = Vec3(m_rayResult.m_tileCoords.x, m_rayResult.m_tileCoords.y, m_rayResult.m_tileCoords.z) + m_rayResult.m_impactNormal * 0.0001f;
	if (m_rayResult.m_impactNormal.y > 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(0, 1, 0) + coords, Vec3(1, 1, 0) + coords, Vec3(1, 1, 1) + coords, Vec3(0, 1, 1) + coords);
	}
	if (m_rayResult.m_impactNormal.y < 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(1, 0, 0) + coords, Vec3(0, 0, 0) + coords, Vec3(0, 0, 1) + coords, Vec3(1, 0, 1) + coords);
	}
	if (m_rayResult.m_impactNormal.x > 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(1, 1, 0) + coords, Vec3(1, 0, 0) + coords, Vec3(1, 0, 1) + coords, Vec3(1, 1, 1) + coords);
	}
	if (m_rayResult.m_impactNormal.x < 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(0, 0, 0) + coords, Vec3(0, 1, 0) + coords, Vec3(0, 1, 1) + coords, Vec3(0, 0, 1) + coords);
	}
	if (m_rayResult.m_impactNormal.z > 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(1, 0, 1) + coords, Vec3(0, 0, 1) + coords, Vec3(0, 1, 1) + coords, Vec3(1, 1, 1) + coords);
	}
	if (m_rayResult.m_impactNormal.z < 0)
	{
		AddVertsForQuad3D(vertsForSelector, Vec3(0, 0, 0) + coords, Vec3(1, 0, 0) + coords, Vec3(1, 1, 0) + coords, Vec3(0, 1, 0) + coords);
	}
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(g_theRenderer->CreateOrGetTextureFromFile("Data/Images/blockSelector.png"));
	g_theRenderer->DrawVertexArray(vertsForSelector);

	if (m_rayFree && m_rayResult.m_didImpact)
	{
		std::vector<Vertex_PCU> rayVerts;
		g_theRenderer->BindTexture(nullptr);
		AddVertsForCylinder3D(rayVerts, m_rayStart, m_rayResult.m_impactPos,0.025f);
		g_theRenderer->DrawVertexArray(rayVerts);
		rayVerts.clear();
		g_theRenderer->SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
		AddVertsForCylinder3D(rayVerts, m_rayResult.m_impactPos, m_rayStart + m_rayFwd * m_rayDist, 0.01f, Rgba8(80, 80, 80,127));
		g_theRenderer->DrawVertexArray(rayVerts);
		g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	}

	g_theRenderer->EndCamera(m_player->m_camera);

	g_theRenderer->BeginCamera(m_screenCamera);
	m_player->Render();
	BlockDef def = BlockDef::s_blockDefRegistry->GetBlockDefByType(m_selectedBlockType);
	
	std::vector<Vertex_PCU> textVerts;
	AABB2 bounds = AABB2(Vec2(0, SCREEN_SIZE_Y - 20.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("DefaultFont");
	font->AddVertsForTextInBox2D(textVerts, bounds, 20.f, "Block : " + def.m_name, Rgba8::WHITE, 1.f, Vec2(0.5f, 0.f));
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(textVerts);
	g_theRenderer->EndCamera(m_screenCamera);

}

void Game::HandleInput()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESCAPE))
	{
		g_theApp->HandleQuitRequested();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_debug = !m_debug;
	}

	std::vector<uint8_t> test;
}

void Game::SetActiveCamera(Camera const& camera)
{
	m_activeCamera = &camera;
}