#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Timer.hpp"

class Game;
class Window;

class App
{
public:
	bool m_newGame			= false;
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();
	void Run();

	bool isQuitting() const { return m_isQuitting; };
	bool HandleQuitRequested();
private:
	void BeginFrame();
	void Update();
	void Render();
	void EndFrame();
private:
	bool  m_firstFrame			= false;
	double m_previousTimeSeconds = 0.;
	double m_currentTimeSeconds  = 0.;

	bool m_isQuitting		= false;
	bool m_isPauseScheduled = false;
	bool m_isPaused			= false;
	bool m_isSlowMo			= false;

	Timer m_gameLoopTimer;
	Game* m_theGame			= nullptr;
	Camera m_consoleCamera;
};