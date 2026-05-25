#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EventSystem.hpp"

#include <string.h>
#include <mutex>

enum class DebugRenderMode
{
	ALWAYS,
	USE_DEPTH,
	X_RAY,
};

struct DebugRenderSystemConfig
{
	Renderer* m_renderer = nullptr;
	std::string m_fontName = "DefaultFont";
};

void DebugRenderSystemSetTarget(Mat44 const& target);

void DebugRenderSystemStartup(DebugRenderSystemConfig const& config);
void DebugRenderSystemShutdown();

void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();
void DebugRenderToggle();

void DebugRenderSystemBeginFrame();
void DebugRenderWorld(Camera const& camera);
void DebugRenderScreen(Camera const& camera);
void DebugRenderSystemEndFrame();

void DebugAddWorldPoint(
	Vec3 const& pos,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddWorldLine(
	Vec3 const& start,
	Vec3 const& end,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddWorldWireCylinder(
	Vec3 const& base,
	Vec3 const& top,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddWorldSphere(
	Vec3 const& center,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddWorldWireSphere(
	Vec3 const& center,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddWorldArrow(
	Vec3 const& start,
	Vec3 const& end,
	float radius,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddWorldText(
	std::string const& text,
	Mat44 const& transform,
	float textHeight,
	Vec2 const& alignment,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddWorldBillboardText(
	std::string const& text,
	Vec3 const& origin,
	float textHeight,
	Vec2 const& alignment,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddWorldBasis(Mat44 const& transform,
	float duration,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH
);

void DebugAddScreenText(
	std::string const& text,
	Vec2 const& position,
	float size,
	Vec2 const& alignment,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE
);

void DebugAddMessage(
	std::string const& text,
	float duration,
	Rgba8 const& startColor = Rgba8::WHITE,
	Rgba8 const& endColor = Rgba8::WHITE
);

bool Command_DebugRenderClear(EventArgs& args);
bool Command_DebugRenderToggle(EventArgs& args);