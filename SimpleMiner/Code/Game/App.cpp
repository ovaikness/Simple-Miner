#include "Game/App.hpp"

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include <cmath>

Renderer*	 g_theRenderer;
InputSystem* g_theInput;
Window*		 g_theWindow;
JobSystem*	 g_theJobSystem;

bool Quit(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return false;
}

App::App()
	: m_gameLoopTimer(0.01666666666666666667f, nullptr)
{
}

App::~App()
{
	delete m_theGame;
	m_theGame = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
}

bool Command_PrintControls(EventArgs& args)
{
	UNUSED(args);
	DevConsole& c = *g_theConsole;
	c.AddLine(Rgba8(240, 160, 40), "Simple Miner");
	c.AddLine(Rgba8(240, 160, 40), "Controls :");
	c.AddLine(Rgba8(240, 160, 40), "Gamepad Left Stick - Move");
	c.AddLine(Rgba8(240, 160, 40), "Gamepad Right Stick - Look Around");
	c.AddLine(Rgba8(240, 160, 40), "Gamepad Left/Right Shoulder - Descend / Elevate");
	c.AddLine(Rgba8(240, 160, 40), "Gamepad Start Button - Reset Position to Origin");
	c.AddLine(Rgba8(240, 160, 40), "Gamepad A Button - Speed up");
	c.AddLine(Rgba8(240, 160, 40), "H - Reset Position to Origin");
	c.AddLine(Rgba8(240, 160, 40), "Shift - Speed up");
	c.AddLine(Rgba8(240, 160, 40), "W - Forward");
	c.AddLine(Rgba8(240, 160, 40), "S - Backward");
	c.AddLine(Rgba8(240, 160, 40), "A - Left");
	c.AddLine(Rgba8(240, 160, 40), "D - Right");
	c.AddLine(Rgba8(240, 160, 40), "Up - Pitch up");
	c.AddLine(Rgba8(240, 160, 40), "Down - Pitch down");
	c.AddLine(Rgba8(240, 160, 40), "Left - Yaw left");
	c.AddLine(Rgba8(240, 160, 40), "Right - Yaw right");
	c.AddLine(Rgba8(240, 160, 40), "Mouse - Look around");
	c.AddLine(Rgba8(240, 160, 40), "P - Pause");
	c.AddLine(Rgba8(240, 160, 40), "A - Step One Frame");
	c.AddLine(Rgba8(240, 160, 40), "T - Time Slow");
	return false;
}

void App::Startup()
{
	g_theJobSystem = new JobSystem();
	g_theJobSystem->Startup();

	g_theInput = new InputSystem();

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_inputSystem = g_theInput;
	windowConfig.m_windowTitle = "Simple Miner";

	g_theWindow	  = new Window(windowConfig);

	RenderConfig renderConfig;
	renderConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(renderConfig);

	g_theWindow->Startup();
	g_theRenderer->Startup();

	DebugRenderSystemConfig config;
	config.m_fontName = "DefaultFont";
	config.m_renderer = g_theRenderer;

	DebugRenderSystemStartup(config);

	m_theGame = new Game();
	m_theGame->Startup();

	g_theEventSystem->SubscribeEventCallbackFunction("quit", Quit);

	DevConsoleConfig devConfig;
	devConfig.m_camera   = &m_consoleCamera;
	devConfig.m_renderer = g_theRenderer;
	EventArgs args;
	g_theConsole = new DevConsole(devConfig);
	g_theConsole->Startup();

	m_consoleCamera.SetOrthoView(Vec2(0, 0), Vec2(1, 1));
	Command_PrintControls(args);
	g_theEventSystem->SubscribeEventCallbackFunction("controls", Command_PrintControls);
}

void App::Shutdown()
{
	DebugRenderSystemShutdown();
	g_theRenderer->Shutdown();
	g_theWindow->Shutdown();
	g_theConsole->Shutdown();
	g_theJobSystem->Shutdown();
}

void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();
	EndFrame();

	//int sleepTime = static_cast<int>(16 - (Clock::GetSystemClock().GetDeltaSeconds() * 1000.));
	////If the deltaSeconds is larger than the frame than we dont need to sleep
	////but do this to prevent casting to an unsigned value to prevent the negative value
	////from being interpreted as a large sleep time
	//if (sleepTime < 0)
	//{
	//	sleepTime = 0;
	//}
	Sleep(0);
}

void App::Run()
{
	while (!isQuitting())
	{
		RunFrame();
	}
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}

void App::BeginFrame()
{
	Clock::TickSystemClock();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theConsole->BeginFrame();
	DebugRenderSystemBeginFrame();
}

void App::Update()
{
	InputSystem& in = *g_theInput;
	if (in.IsKeyDown('T'))
	{
		m_theGame->m_gameClock.SetTimeScale(0.1f);
	}
	else
	{
		m_theGame->m_gameClock.SetTimeScale(1.f);
	}

	if (in.WasKeyJustPressed('P'))
	{
		Clock::GetSystemClock().TogglePause();
	}
	if (in.WasKeyJustPressed('O'))
	{
		Clock::GetSystemClock().StepSingleFrame();
	}
	if (g_theConsole->GetMode() != HIDDEN)
	{
		g_theInput->SetCursorState(false, false);
		return;
	}
	//Run game update logic, after evaluating update conditions.
	m_theGame->Update();
}

void App::Render()
{
	g_theRenderer->ClearScreen(Rgba8(40,40,40));
	m_theGame->Render();

	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->BeginCamera(m_consoleCamera);
	g_theConsole->Render(AABB2(0, 0, 1, 1));
	g_theRenderer->EndCamera(m_consoleCamera);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theConsole->EndFrame();
	DebugRenderSystemEndFrame();
}