#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

enum class CursorIcon
{
	POINTER,
	HAND,
	CURSOR_ICON_COUNT
};

class InputSystem;
class Camera;

struct WindowConfig
{
	InputSystem* m_inputSystem = nullptr;
	bool		 m_fullscreen = false;
	Vec2		 m_size = Vec2(-1, -1);
	Vec2		 m_position = Vec2(-1, -1);
	float		 m_aspectRatio = 2.f;
	std::string  m_windowTitle = "UNNAMED APPLICATION";
};

class Window
{
public:
	void* m_cursorHandle	= nullptr;
public:
	Window(WindowConfig const& config);
	Vec3 GetMouseForward(float degrees, Camera const& camera) const;
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void OpenFileNextFrame(char const* filePattern = "*.*");
	void SetCursorIcon(CursorIcon icon);
	Vec2 GetNormalizedCursorPos() const;
	IntVec2 GetClientDimensions() const;
	void* GetHwnd() const;

	WindowConfig const&		GetConfig() const;
	float				    GetAspect() const;
	void*					GetDisplayContext() const;

	static Window*			GetMainWindowInstance();
protected:
	void SetCursorIconImplementation();
	void CreateOSWindow();
	void RunMessagePump();

protected:
	static Window*			s_theWindow;

protected:
	bool 				    m_openFileNextFrame = false;
	std::string				m_nextFrameFilePattern;
	CursorIcon				m_cursorIcon = CursorIcon::POINTER;
	WindowConfig			m_config;
	IntVec2					m_dimensions;
	void*					m_windowHandle   = nullptr;
	void*					m_displayContext = nullptr;
};