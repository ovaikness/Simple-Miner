#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Timer.hpp"
#include <string.h>
#include <vector>
#include <mutex>

enum DevConsoleMode
{
	HIDDEN,
	DEFAULT,
};

struct DevConsoleConfig
{
	DevConsoleConfig()       = default;
	Renderer*   m_renderer   = nullptr;
	Camera*     m_camera     = nullptr;
	std::string m_fontName   = "DefaultFont";
	float       m_fontAspect = 0.5f;
	int         m_linesOnScreen     = 40;
	int         m_maxCommandHistory = 128;
	bool        m_startOpen = false;
};

struct DevConsoleLine
{
	Rgba8	    m_color;
	int			m_frameNumber;
	std::string m_text;
	DevConsoleLine(Rgba8 const& color, std::string const& text, int frameNumber)
		: m_color(color)
		, m_text(text)
		, m_frameNumber(frameNumber)
	{
	}
};

#undef ERROR
class DevConsole
{
public:
	static Rgba8 const ERROR;
	static Rgba8 const WARNING;
	static Rgba8 const INFO_MAJOR;
	static Rgba8 const INFO_MINOR;
public:
	~DevConsole();
	DevConsole(DevConsoleConfig const& config);

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void ExecuteXmlCommandScriptNode(XmlElement const& element);
	void ExecuteCommandScriptFile(std::string const& filePath);
	void Execute(std::string const& consoleCommandText);
	void AddLine(Rgba8 const& color, std::string const& text);
	void Render(AABB2 const& bounds, Renderer* rendererOverride = nullptr);

	static bool Event_KeyPressed(EventArgs& args);
	static bool Event_CharInput(EventArgs& args);

	static bool Command_Clear(EventArgs& args);
	static bool Command_Help(EventArgs& args);
	static bool Command_Echo(EventArgs& args);

	DevConsoleMode GetMode() const;
	void SetMode(DevConsoleMode mode);
	void ToggleMode(DevConsoleMode mode);
protected:
	void AddCharAtInsertionPoint(char c);
	void RemoveCharBeforeInsertionPoint();
	void RemoveCharAtInsertionPoint();
	void Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect = 1.f);
protected:
	mutable std::recursive_mutex m_mutex;
	DevConsoleConfig m_config;
	DevConsoleMode m_mode = HIDDEN;
	std::vector<DevConsoleLine> m_lines;
	int m_frameNumber = 0;
	std::string m_inputText = "";

	int m_insertionPointPosition = 0;
	bool m_insertionPointVisible = true;
	Timer m_insertionPointTimer = Timer(0.5f);

	std::vector<std::string> m_commandHistory;

	int m_historyIndex = -1;
};

extern DevConsole* g_theConsole;
