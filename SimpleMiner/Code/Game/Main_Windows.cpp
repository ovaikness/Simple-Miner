#include "Game/App.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <iostream>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

App* g_theApp;

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int nShowCmd )
{
	UNUSED( applicationInstanceHandle );
	UNUSED( nShowCmd);
	UNUSED( commandLineString );

	g_theApp = new App();
	g_theApp->Startup();
	g_theApp->Run();
	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}


