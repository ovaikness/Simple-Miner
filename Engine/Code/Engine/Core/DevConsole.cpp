#include "DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/TinyXMLUtils.hpp"

DevConsole* g_theConsole = nullptr;

Rgba8 const DevConsole::ERROR      = Rgba8(255, 0, 0);
Rgba8 const DevConsole::WARNING    = Rgba8(255, 255, 0);
Rgba8 const DevConsole::INFO_MAJOR = Rgba8(127, 127, 255);
Rgba8 const DevConsole::INFO_MINOR = Rgba8(80, 80, 127);

DevConsole::~DevConsole()
{

}

DevConsole::DevConsole(DevConsoleConfig const& config)
{
	m_config = config;
}

void DevConsole::Startup()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	AddLine(Rgba8::WHITE, "Welcome to the dev console!");
	g_theEventSystem->SubscribeEventCallbackFunction("Help",  Command_Help);
	g_theEventSystem->SubscribeEventCallbackFunction("Clear", Command_Clear);
	g_theEventSystem->SubscribeEventCallbackFunction("Echo",  Command_Echo);
	g_theEventSystem->SubscribeEventCallbackFunction("KeyTyped",   Event_CharInput);
	g_theEventSystem->SubscribeEventCallbackFunction("KeyPressed", Event_KeyPressed);
}

void DevConsole::Shutdown()
{
	delete g_theConsole;
	g_theConsole = nullptr;
}

void DevConsole::BeginFrame()
{
	++m_frameNumber;
}

void DevConsole::EndFrame()
{

}

void DevConsole::ExecuteXmlCommandScriptNode(XmlElement const& root)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	for (XmlElement const* element = root.FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
	{
		std::string commandName = element->Name();
		if (commandName != "")
		{
			EventArgs args;
			for (XmlAttribute const* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				args.SetValue(attr->Name(), attr->Value());
			}
			g_theEventSystem->FireEvent(commandName, args);
		}
	}
}

void DevConsole::ExecuteCommandScriptFile(std::string const& filepath)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	XmlDocument doc;
	if (doc.LoadFile(filepath.c_str()) == tinyxml2::XML_SUCCESS)
	{
		XmlElement* root = doc.RootElement();
		ExecuteXmlCommandScriptNode(*root);
	}
	else
	{
		AddLine(Rgba8(255, 0, 0, 255), "Failed to load command script file: " + filepath);
	}
}


void DevConsole::Execute(std::string const& consoleCommandText)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_insertionPointPosition = 0;
	if (consoleCommandText == "")
	{
		ToggleMode(HIDDEN);
		return;
	}
	m_commandHistory.push_back(consoleCommandText);
	if (m_commandHistory.size() > m_config.m_maxCommandHistory)
	{
		std::vector<std::string> newHistory = std::vector<std::string>(m_commandHistory.begin() + 1, m_commandHistory.end());
		m_commandHistory = newHistory;
	}

	Strings commandLines = SplitStringOnDelimiter(consoleCommandText, '\n');
	for (std::string const& commandLine : commandLines)
	{
		Strings argsPairs = SplitStringOnDelimiterWithQuotes(commandLine, ' ');
		std::string eventName = argsPairs[0];
		EventArgs eventArgs;
		for (int index = 1; index < argsPairs.size(); ++index)
		{
			std::string argsPair = argsPairs[index];
			Strings     args = SplitStringOnDelimiterWithQuotes(argsPair, '=');
			if (args.size() > 1)
			{
				eventArgs.SetValue(args[0], args[1]);
			}
		}

		if (!g_theEventSystem->HasEvent(eventName))
		{
			AddLine(Rgba8(255, 0, 0, 255), "Unrecognized command : " + consoleCommandText);
		}
		else
		{
			AddLine(Rgba8(150, 150, 255), consoleCommandText);
		}

		g_theEventSystem->FireEvent(eventName, eventArgs);
		m_historyIndex = -1;
	}
}

void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_lines.push_back(DevConsoleLine(color, text, m_frameNumber));
}

void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverride /*= nullptr*/)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	UNUSED(bounds);
	if (m_mode == HIDDEN)
	{
		return;
	}
	if (rendererOverride)
	{
		BitmapFont* consoleFont = m_config.m_renderer->GetBitmapFontForFilename(m_config.m_fontName.c_str());
		Render_OpenFull(m_config.m_camera->GetView(), *rendererOverride, *consoleFont, m_config.m_fontAspect);
	}
	else
	{
		BitmapFont* consoleFont = m_config.m_renderer->GetBitmapFontForFilename(m_config.m_fontName.c_str());
		Render_OpenFull(m_config.m_camera->GetView(), *m_config.m_renderer,*consoleFont, m_config.m_fontAspect);
	}
}

bool DevConsole::Event_KeyPressed(EventArgs& args)
{
	std::lock_guard<std::recursive_mutex> lock(g_theConsole->m_mutex);
	unsigned char c = args.GetValue("Key", (char)-1);
	if (c != (char)-1)
	{
		if (c == KEYCODE_TILDE)
		{
			g_theConsole->ToggleMode(HIDDEN);
			return true;
		}

		if (g_theConsole->m_mode == HIDDEN)
		{
			return false;
		}

		if (c == KEYCODE_DELETE)
		{
			g_theConsole->RemoveCharAtInsertionPoint();
			return true;
		}
		if (c == KEYCODE_RIGHT)
		{
			g_theConsole->m_insertionPointPosition++;
			if (g_theConsole->m_inputText.size() < g_theConsole->m_insertionPointPosition)
			{
				g_theConsole->m_insertionPointPosition = static_cast<int>(g_theConsole->m_inputText.size());
			}
			return true;
		}
		if (c == KEYCODE_LEFT)
		{
			g_theConsole->m_insertionPointPosition--;
			if (g_theConsole->m_insertionPointPosition < 0)
			{
				g_theConsole->m_insertionPointPosition = 0;
			}
			return true;
		}

		if (c == KEYCODE_HOME)
		{
			g_theConsole->m_insertionPointPosition = 0;
			return true;
		}
		if (c == KEYCODE_END)
		{
			g_theConsole->m_insertionPointPosition = static_cast<int>(g_theConsole->m_inputText.size());
			return true;
		}

		if (c == KEYCODE_ESCAPE && g_theConsole->GetMode() == DEFAULT)
		{
			g_theConsole->ToggleMode(HIDDEN);
			return true;
		}
		if (g_theConsole->GetMode() == HIDDEN)
		{
			return false;
		}
		if (c == KEYCODE_ENTER)
		{
			std::string& in = g_theConsole->m_inputText;
			g_theConsole->Execute(in);
			in = "";
			return true;
		}
		if (c == KEYCODE_UP)
		{
			g_theConsole->m_historyIndex++;
			size_t size = g_theConsole->m_commandHistory.size();
			if (size > 0)
			{
				if (g_theConsole->m_historyIndex > size - 1)
				{
					g_theConsole->m_historyIndex = (int)(size - 1);
				}

				g_theConsole->m_inputText = g_theConsole->m_commandHistory[size - (int)1 - g_theConsole->m_historyIndex];
			}
			g_theConsole->m_insertionPointPosition = static_cast<int>(g_theConsole->m_inputText.size());
			return true;
		}
		if (c == KEYCODE_DOWN)
		{
			g_theConsole->m_historyIndex--;
			if (g_theConsole->m_historyIndex < 0)
			{
				g_theConsole->m_historyIndex = -1;
				g_theConsole->m_inputText = "";
				g_theConsole->m_insertionPointPosition = 0;
				return true;
			}

			size_t size = g_theConsole->m_commandHistory.size();
			if (size > 0)
			{
				g_theConsole->m_inputText = g_theConsole->m_commandHistory[size - (size_t)1 - g_theConsole->m_historyIndex];
			}
			g_theConsole->m_insertionPointPosition = static_cast<int>(g_theConsole->m_inputText.size());
			return true;
		}
		return true;
	}
	return false;
}

bool DevConsole::Event_CharInput(EventArgs& args)
{
	std::lock_guard<std::recursive_mutex> lock(g_theConsole->m_mutex);
	if (g_theConsole->GetMode() == HIDDEN)
	{
		return false;
	}
	unsigned char c;
	c = args.GetValue("Key", (char)-1);

	if (c == KEYCODE_BACKSPACE && g_theConsole->m_inputText.size() > 0)
	{
		g_theConsole->RemoveCharBeforeInsertionPoint();
		return true;
	}

	if (c < 32 || c > 126)
	{
		return false;
	}

	if (c != (char)-1 && c != KEYCODE_BACKSPACE && c != KEYCODE_ENTER && c != '`')
	{
		g_theConsole->AddCharAtInsertionPoint(c);
		return true;
	}

	return false;
}

bool DevConsole::Command_Echo(EventArgs& args)
{
	g_theConsole->AddLine(Rgba8(255,255,0), args.GetValue("message",""));
	return false;
}

bool DevConsole::Command_Clear(EventArgs& args)
{
	UNUSED(args);
	g_theConsole->m_mutex.lock();
	g_theConsole->m_lines.clear();
	g_theConsole->m_mutex.unlock();
	return false;
}

bool DevConsole::Command_Help(EventArgs& args)
{
	g_theConsole->m_mutex.lock();
	UNUSED(args);
	Strings eventNames = g_theEventSystem->GetEventNames();
	g_theConsole->AddLine(Rgba8::WHITE, "Registered Events : ");
	for (std::string const& eventName : eventNames)
	{
		g_theConsole->AddLine(Rgba8::WHITE, eventName);
	}
	g_theConsole->m_mutex.unlock();
	return false;
}

DevConsoleMode DevConsole::GetMode() const
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_mode;
}

void DevConsole::SetMode(DevConsoleMode mode)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_mode = mode;
}

void DevConsole::ToggleMode(DevConsoleMode mode)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (m_mode == mode)
	{
		m_mode = DEFAULT;
	}
	else
	{
		m_mode = mode;
	}
}

void DevConsole::AddCharAtInsertionPoint(char c)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::string begin = std::string(m_inputText.begin(), m_inputText.begin() + m_insertionPointPosition);
	std::string end = std::string(m_inputText.begin() + m_insertionPointPosition, m_inputText.end());
	m_inputText = begin + c + end;
	m_insertionPointPosition++;
}

void DevConsole::RemoveCharBeforeInsertionPoint()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (m_insertionPointPosition == 0)
	{
		return;
	}
	if (m_inputText.begin() != m_inputText.end())
	{
		m_inputText.erase(m_inputText.begin() + m_insertionPointPosition - 1);
		m_insertionPointPosition--;
		if (m_insertionPointPosition < 0)
		{
			m_insertionPointPosition = 0;
		}
	}
}

void DevConsole::RemoveCharAtInsertionPoint()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (m_inputText.begin() != m_inputText.end())
	{
		m_inputText.erase(m_inputText.begin() + m_insertionPointPosition);
	}
}

void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect /*= 1.f*/)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	renderer.BindShader(nullptr);
	renderer.BindTexture(nullptr);
	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.BeginCamera(*m_config.m_camera);
	std::vector<Vertex_PCU> quad;
	AddVertsForAABB2D(quad, Vec2(0, 0), Rgba8(0, 0, 0, 127), bounds);
	renderer.DrawVertexArray(quad);

	std::vector<Vertex_PCU> verts;
	float boundsHeight = (bounds.m_max.y - bounds.m_min.y);
	float cellHeight   = boundsHeight / static_cast<float>(m_config.m_linesOnScreen);
	float cellWidth    = bounds.m_max.x - bounds.m_min.x;
	Vec2  begin = Vec2(bounds.m_min.x, bounds.m_min.y);
	int lineCountMax = static_cast<int>(boundsHeight / cellHeight) + 1;
	int lineCount = 0;

	//Draw input
	font.AddVertsForTextInBox2D(
		verts, 
		AABB2(
			begin + Vec2(0, 0),
			begin + Vec2(cellWidth, cellHeight)
		),
		cellHeight,
		m_inputText,
		Rgba8::WHITE,
		fontAspect,
		Vec2(0.0f, 0.f),
		OVERRUN
	);


	for (int index = static_cast<int>(m_lines.size() - 1); index >= 0 && lineCount < lineCountMax; --index)
	{
		int const&		   frame = m_lines[index].m_frameNumber;
		std::string const& text  = m_lines[index].m_text;
		Rgba8 const&       color = m_lines[index].m_color;
		//Add one because of the input line
		int indexOffset = static_cast<int>(m_lines.size() - 1 - index) + 1;
		font.AddVertsForTextInBox2D(
			verts,
			AABB2(
				begin + Vec2(0, cellHeight * indexOffset),
				begin + Vec2(cellWidth, cellHeight * (indexOffset + 1))
			),
			cellHeight,
			std::to_string(frame) + " : " + text,
			color,
			fontAspect,
			Vec2(0.0f,0.f),
			OVERRUN
		);
		lineCount++;
	}
	renderer.BindTexture(&font.GetTexture());
	renderer.DrawVertexArray(verts);
	renderer.BindTexture(nullptr);

	std::vector<Vertex_PCU> indicatorVerts;

	Vec2 anchor = font.GetDenormalizedAnchorForBox(AABB2(
		begin + Vec2(0, 0),
		begin + Vec2(cellWidth,cellHeight)
	), Vec2(0.f, 0.f));

	Vec2 translation = Vec2(cellHeight * fontAspect * m_insertionPointPosition, 0) + anchor;
	AABB2 indicatorAABB = AABB2(
		begin + Vec2(0, 0),
		begin + Vec2(cellHeight * fontAspect * 0.2f, cellHeight)
	);

	while (m_insertionPointTimer.DecrementPeriodIfElapsed())
	{
		m_insertionPointVisible = !m_insertionPointVisible;
	}

	if (m_insertionPointVisible)
	{
		indicatorAABB.Translate(translation);
		AddVertsForAABB2D(indicatorVerts, Vec2::ZERO, Rgba8::WHITE, indicatorAABB);
		renderer.DrawVertexArray(indicatorVerts);
	}

	renderer.EndCamera(*m_config.m_camera);
}

