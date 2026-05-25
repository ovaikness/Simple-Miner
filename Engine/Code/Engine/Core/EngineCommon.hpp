#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#define UNUSED(x) (void)(x);

class DevConsole;

extern EventSystem* g_theEventSystem;
extern DevConsole*  g_theConsole;
extern NamedStrings g_gameConfigBlackboard;