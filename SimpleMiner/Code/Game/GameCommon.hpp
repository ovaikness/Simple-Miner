#pragma once
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Threads/JobSystem.hpp"

class Game;
class App;
class Renderer;
class InputSystem;
class Window;
struct Rgba8;
struct Vec2;

enum class EntityType
{
	ENTITY_TYPE_NUM
};

constexpr unsigned int MAX_POINT_LIGHT_2DS = 200;

constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float SCREEN_CENTER_X = 800.f;
constexpr float SCREEN_CENTER_Y = 400.f;

void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void DebugDrawRing(Vec2 position, float radius, float thickness, Rgba8 color);

extern Game* g_theGame;
extern RandomNumberGenerator g_RNG;
extern Renderer* g_theRenderer;
extern App* g_theApp;
extern InputSystem* g_theInput;
extern Window* g_theWindow;
extern JobSystem* g_theJobSystem;