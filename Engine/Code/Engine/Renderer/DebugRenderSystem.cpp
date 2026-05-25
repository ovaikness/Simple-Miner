#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <mutex>

class DebugRenderSystem;

class DebugRenderEntity
{
	friend class DebugRenderSystem;
public:
	~DebugRenderEntity();
	DebugRenderEntity(
		DebugRenderSystem& system
		, VertexBuffer* vbo
		, float duration
		, int count
		, Rgba8 startColor
		, Rgba8 endColor
		, DebugRenderMode mode
		, bool wireframe = false
		, BillboardType billboardType = BillboardType::NONE
	);

	void FixedUpdate();
	void Render();
protected:
	bool m_wireframe;
	bool m_isGarbage;
	BillboardType m_billboardType;
	DebugRenderMode m_mode;
	DebugRenderSystem& m_system;
	int   m_count;
	Mat44 m_modelMatrix;
	Timer m_timer;
	VertexBuffer* m_vbo;
	Rgba8 m_startColor;
	Rgba8 m_endColor;

	Texture* m_texture = nullptr;
};

class DebugRenderSystem
{
public:
	Mat44 m_target = Mat44::IDENTITY;
public:
	~DebugRenderSystem();
	DebugRenderSystem(DebugRenderSystemConfig const& config);


	DebugRenderSystemConfig GetConfig();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void AddDebugRenderMessageEntity(DebugRenderEntity* entity);
	void AddDebugScreenRenderEntity(DebugRenderEntity* entity);
	void AddDebugRenderEntity(DebugRenderEntity* entity);

	void AddWorldPoint(
		Vec3 const& pos,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

	void AddWorldLine(
		Vec3 const& start,
		Vec3 const& end,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

	void AddWorldSphere(
		Vec3 const& center,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddWorldWireCylinder(
		Vec3 const& base,
		Vec3 const& top,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

	void AddWorldWireSphere(
		Vec3 const& center,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddWorldArrow(
		Vec3 const& start,
		Vec3 const& end,
		float radius,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddWorldText(
		std::string const& text,
		Mat44 const& transform,
		float textHeight,
		Vec2 const& alignment,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddWorldBillboardText(
		std::string const& text,
		Vec3 const& origin,
		float textHeight,
		Vec2 const& alignment,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddWorldBasis(Mat44 const& transform,
		float duration,
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH
	);

	void AddScreenText(
		std::string const& text,
		Vec2 const& position,
		float size,
		Vec2 const& alignment,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE
	);

	void AddMessage(
		std::string const& text,
		float duration,
		Rgba8 const& startColor = Rgba8::WHITE,
		Rgba8 const& endColor = Rgba8::WHITE
	);

	void Toggle();
	void Clear();

	void SetHidden();
	void SetVisible();

	void WorldRender();
	void ScreenRender();
protected:
	bool m_visible;
	DebugRenderSystemConfig m_config;
	std::vector<DebugRenderEntity*> m_entities;
	std::vector<DebugRenderEntity*> m_screenEntities;
	std::vector<DebugRenderEntity*> m_messageEntities;
	std::recursive_mutex m_mutex;
};

DebugRenderSystem* s_debugRenderSystem = nullptr;

void DebugRenderSystemSetTarget(Mat44 const& target)
{
	s_debugRenderSystem->m_target = target;
}

void DebugRenderSystemStartup(DebugRenderSystemConfig const& config)
{
	if (s_debugRenderSystem == nullptr)
	{
		s_debugRenderSystem = new DebugRenderSystem(config);
	}
	else
	{
		ERROR_AND_DIE("Attempted to start the debug render system twice!");
	}

	g_theEventSystem->SubscribeEventCallbackFunction("debugRenderClear", Command_DebugRenderClear);
	g_theEventSystem->SubscribeEventCallbackFunction("debugRenderToggle", Command_DebugRenderToggle);
}

void DebugRenderSystemShutdown()
{
	if (s_debugRenderSystem)
	{
		s_debugRenderSystem->Shutdown();
	}
}

void DebugRenderSetVisible()
{
	s_debugRenderSystem->SetVisible();
}

void DebugRenderSetHidden()
{
	s_debugRenderSystem->SetHidden();
}

void DebugRenderClear()
{
	s_debugRenderSystem->Clear();
}

void DebugRenderToggle()
{
	s_debugRenderSystem->Toggle();
}

void DebugRenderSystemBeginFrame()
{
	s_debugRenderSystem->BeginFrame();
}

void DebugRenderWorld(Camera const& camera)
{
	Renderer& renderer = *s_debugRenderSystem->GetConfig().m_renderer;
	renderer.BeginCamera(camera);
	s_debugRenderSystem->WorldRender();
	renderer.EndCamera(camera);
}

void DebugRenderScreen(Camera const& camera)
{
	Camera screenCamera;
	AABB2 view = camera.GetView();
	screenCamera.SetOrthoView(Vec2(0, 0), Vec2(200, 100));

	Renderer& renderer = *s_debugRenderSystem->GetConfig().m_renderer;
	renderer.BeginCamera(screenCamera);
	s_debugRenderSystem->ScreenRender();
	renderer.EndCamera(screenCamera);
}

void DebugRenderSystemEndFrame()
{
	s_debugRenderSystem->EndFrame();
}

void DebugAddWorldPoint(Vec3 const& pos, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldPoint(
		pos,
		radius,
		duration,
		startColor,
		endColor,
		mode
	);
}

void DebugAddWorldLine(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldLine(
		start,
		end,
		radius,
		duration,
		startColor,
		endColor,
		mode
	);
}

void DebugAddWorldWireCylinder(Vec3 const& base, Vec3 const& top, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldWireCylinder(
		base,
		top,
		radius,
		duration,
		startColor,
		endColor,
		mode
	);
}

void DebugAddWorldWireSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldWireSphere(center, radius, duration, startColor, endColor, mode);
}

void DebugAddWorldSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldSphere(center, radius, duration, startColor, endColor, mode);
}

void DebugAddWorldArrow(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldArrow(
		start,
		end,
		radius,
		duration,
		startColor,
		endColor,
		mode
	);
}

void DebugAddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldText(text,
		transform,
		textHeight,
		alignment,
		duration,
		startColor,
		endColor,
		mode);
}

void DebugAddWorldBillboardText(std::string const& text, Vec3 const& origin, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldBillboardText(
		text,
		origin,
		textHeight,
		alignment,
		duration,
		startColor,
		endColor,
		mode
	);
}

void DebugAddWorldBasis(Mat44 const& transform, float duration, DebugRenderMode mode)
{
	s_debugRenderSystem->AddWorldBasis(transform, duration, mode);
}

void DebugAddScreenText(std::string const& text, Vec2 const& position, float size, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	s_debugRenderSystem->AddScreenText(text, position, size, alignment, duration, startColor, endColor);
}

void DebugAddMessage(std::string const& text, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	s_debugRenderSystem->AddMessage(text, duration, startColor, endColor);
}

bool Command_DebugRenderClear(EventArgs& args)
{
	UNUSED(args);
	s_debugRenderSystem->Clear();
	return false;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	UNUSED(args);
	s_debugRenderSystem->Toggle();
	return false;
}

DebugRenderSystem::~DebugRenderSystem()
{
	Clear();
}

DebugRenderSystem::DebugRenderSystem(DebugRenderSystemConfig const& config)
	: m_config(config)
	, m_visible(true)
{

}

DebugRenderSystemConfig DebugRenderSystem::GetConfig()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_config;
}

void DebugRenderSystem::BeginFrame()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_entities)
	{
		if (e)
		{
			e->FixedUpdate();
		}
	}
	for (DebugRenderEntity*& e : m_screenEntities)
	{
		if (e)
		{
			e->FixedUpdate();
		}
	}

	for (DebugRenderEntity*& e : m_messageEntities)
	{
		if (e)
		{
			e->FixedUpdate();
		}
	}

	int index = 0;
	for (DebugRenderEntity*& e : m_messageEntities)
	{
		if (e)
		{
			e->m_modelMatrix.SetTranslation3D(Vec3(0, 100.f + (index++ + 3) * -1.2f, 0.f));
		}
	}
}

void DebugRenderSystem::EndFrame()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_entities)
	{
		if (e)
		{
			if (e->m_isGarbage || e->m_timer.m_period == 0.f)
			{
				delete e;
				e = nullptr;
			}
		}
	}
	for (DebugRenderEntity*& e : m_screenEntities)
	{
		if (e)
		{
			if (e->m_isGarbage || e->m_timer.m_period == 0.f)
			{
				delete e;
				e = nullptr;
			}
		}
	}

	for (DebugRenderEntity*& e : m_messageEntities)
	{
		if (e)
		{
			if (e->m_isGarbage || e->m_timer.m_period == 0.f)
			{
				delete e;
				e = nullptr;
			}
		}
	}
}

void DebugRenderSystem::Shutdown()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Clear();
}

void DebugRenderSystem::AddDebugRenderMessageEntity(DebugRenderEntity* entity)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_messageEntities)
	{
		if (e == nullptr)
		{
			e = entity;
			return;
		}
	}
	m_messageEntities.push_back(entity);
	int index = 0;
	for (DebugRenderEntity*& e : m_messageEntities)
	{
		if (e)
		{
			e->m_modelMatrix.SetTranslation3D(Vec3(0, 100.f + (index++ + 3) * -1.2f, 0.f));
		}
	}
}

void DebugRenderSystem::AddDebugScreenRenderEntity(DebugRenderEntity* entity)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_screenEntities)
	{
		if (e == nullptr)
		{
			e = entity;
			return;
		}
	}
	m_screenEntities.push_back(entity);
}

void DebugRenderSystem::AddDebugRenderEntity(DebugRenderEntity* entity)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_entities)
	{
		if (e == nullptr)
		{
			e = entity;
			return;
		}
	}
	m_entities.push_back(entity);
}

void DebugRenderSystem::AddWorldPoint(Vec3 const& pos, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH*/)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	AddVertsForSphere3D(vertices, Vec3(pos.x, pos.y, 0.f), radius);
	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldLine(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH*/)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	Vec3 forward = end - start;
	forward.Normalize();

	AddVertsForCylinder3D(vertices,
		start,
		end,
		radius,
		Rgba8::WHITE,
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldWireCylinder(Vec3 const& base, Vec3 const& top, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH*/)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	AddVertsForCylinder3D(vertices,
		base,
		top,
		radius,
		Rgba8::WHITE,
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode,
		true
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldWireSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	AddVertsForSphere3D(vertices, Vec3(center.x, center.y, center.z), radius);
	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode,
		true
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	AddVertsForSphere3D(vertices, Vec3(center.x, center.y, center.z), radius);
	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode,
		false
	);
	AddDebugRenderEntity(entity);
}
void DebugRenderSystem::AddWorldArrow(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	Vec3 forward = end - start;
	forward.Normalize();

	AddVertsForCylinder3D(vertices,
		start,
		end,
		radius,
		Rgba8::WHITE,
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	AddVertsForCone3D(vertices,
		end,
		end + forward * radius * 2.f,
		radius + radius * 0.5f,
		Rgba8::WHITE,
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Renderer& renderer = *m_config.m_renderer;
	std::vector<Vertex_PCU> vertices;
	BitmapFont* font = renderer.CreateOrGetBitmapFont(m_config.m_fontName.c_str());
	font->AddVertsForTextInBox2D(
		vertices, 
		AABB2(Vec2(0.f, 0.f), 
		Vec2(10.f, 1.f)), 
		textHeight, 
		text,
		Rgba8::WHITE,
		1.f,
		alignment);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode
	);
	entity->m_modelMatrix = transform;
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldBillboardText(std::string const& text, Vec3 const& origin, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Renderer& renderer = *m_config.m_renderer;
	std::vector<Vertex_PCU> vertices;
	BitmapFont* font = renderer.CreateOrGetBitmapFont(m_config.m_fontName.c_str());
	float width = font->GetTextWidth(textHeight, text);
	font->AddVertsForTextInBox2D(
		vertices,
		AABB2(Vec2(width * -.5f, textHeight * -0.5f),
			Vec2(width * .5f, textHeight * 0.5f)),
		textHeight,
		text,
		Rgba8::WHITE,
		1.f,
		alignment);

	EulerAngles rotation = EulerAngles(90,0,90);
	Mat44 transform = rotation.GetAsMatrix_IFwd_JLeft_KUp();
	TransformVertexArray3D(vertices, transform);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		mode,
		false,
		BillboardType::FULL_OPPOSING
	);

	entity->m_modelMatrix = Mat44::CreateTranslation3D(origin);

	entity->m_texture = &font->GetTexture();

	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddWorldBasis(Mat44 const& transform, float duration, DebugRenderMode mode /*= USE_DEPTH */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	std::vector<Vertex_PCU> vertices;
	Vec3 start = transform.GetTranslation3D();
	Vec3 iBasis = transform.GetIBasis3D();
	Vec3 jBasis = transform.GetJBasis3D();
	Vec3 kBasis = transform.GetKBasis3D();
	float radius = 0.05f;
	float averageScale = (transform.m_Values[Mat44::Ix] + transform.m_Values[Mat44::Jy] + transform.m_Values[Mat44::Kz]) / 3.f;
	//iBasis
	AddVertsForCylinder3D(vertices,
		start,
		start + iBasis * 1.f,
		radius  * averageScale,
		Rgba8(255,0,0),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);
	AddVertsForCone3D(vertices,
		(start + iBasis * 1.f),
		(start + iBasis * 1.f) + iBasis * radius * 2.f,
		(radius + radius * 0.5f) * averageScale,
		Rgba8(255,0,0),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	//jBasis
	AddVertsForCylinder3D(vertices,
		start,
		start + jBasis * 1.f,
		radius * averageScale,
		Rgba8(0,255, 0),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);
	AddVertsForCone3D(vertices,
		(start + jBasis * 1.f),
		(start + jBasis * 1.f) + jBasis * radius * 2.f,
		(radius + radius * 0.5f) * averageScale,
		Rgba8(0, 255, 0),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	//kBasis
	AddVertsForCylinder3D(vertices,
		start,
		start + kBasis * 1.f,
		radius * averageScale,
		Rgba8(0, 0, 255),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);
	AddVertsForCone3D(vertices,
		(start + kBasis * 1.f),
		(start + kBasis * 1.f) + kBasis * radius * 2.f,
		(radius + radius * 0.5f) * averageScale,
		Rgba8(0, 0,255),
		AABB2(Vec2::ZERO, Vec2(1.f, 1.f))
	);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		Rgba8::WHITE,
		Rgba8::WHITE,
		mode
	);
	AddDebugRenderEntity(entity);
}

void DebugRenderSystem::AddScreenText(std::string const& text, Vec2 const& position, float size, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= WHITE*/, Rgba8 const& endColor /*= WHITE */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Renderer& renderer = *m_config.m_renderer;
	std::vector<Vertex_PCU> vertices;
	BitmapFont* font = renderer.CreateOrGetBitmapFont(m_config.m_fontName.c_str());
	float width = font->GetTextWidth(size, text);
	font->AddVertsForTextInBox2D(
		vertices,
		AABB2(position,
			position + Vec2(width, 1.f)),
		size,
		text,
		Rgba8::WHITE,
		1.f,
		alignment,
		TextBoxMode::OVERRUN);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		DebugRenderMode::ALWAYS
	);

	entity->m_texture = &font->GetTexture();

	AddDebugScreenRenderEntity(entity);
}

void DebugRenderSystem::AddMessage(std::string const& text, float duration, Rgba8 const& startColor /*= WHITE*/, Rgba8 const& endColor /*= WHITE */)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Renderer& renderer = *m_config.m_renderer;
	std::vector<Vertex_PCU> vertices;
	BitmapFont* font = renderer.CreateOrGetBitmapFont(m_config.m_fontName.c_str());
	font->AddVertsForTextInBox2D(
		vertices,
		AABB2(Vec2(0,0),
			1.f * Vec2(100.f, 1.f)),
		1.2f,
		text,
		Rgba8::WHITE,
		1.2f,
		Vec2(0.f, 0.f),
		OVERRUN);

	VertexBuffer* buffer = m_config.m_renderer->CreateVertexBufferFromVertexPCUArray(vertices);
	DebugRenderEntity* entity = new DebugRenderEntity(
		*this,
		buffer,
		duration,
		static_cast<int>(vertices.size()),
		startColor,
		endColor,
		DebugRenderMode::ALWAYS
	);

	entity->m_texture = &font->GetTexture();

	AddDebugRenderMessageEntity(entity);
}

void DebugRenderSystem::Toggle()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_visible = !m_visible;
}

void DebugRenderSystem::Clear()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (DebugRenderEntity*& e : m_entities)
	{
		delete e;
		e = nullptr;
	}

	for (DebugRenderEntity*& e : m_screenEntities)
	{
		delete e;
		e = nullptr;
	}

	for (DebugRenderEntity*& e : m_messageEntities)
	{
		delete e;
		e = nullptr;
	}
}

void DebugRenderSystem::SetHidden()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_visible = false;
}

void DebugRenderSystem::SetVisible()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_visible = true;
}

void DebugRenderSystem::WorldRender()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (!m_visible)
	{
		return;
	}

	for (DebugRenderEntity* e : m_entities)
	{
		if (e)
		{
			e->Render();
		}
	}
}

void DebugRenderSystem::ScreenRender()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (!m_visible)
	{
		return;
	}

	for (DebugRenderEntity* e : m_screenEntities)
	{
		if (e)
		{
			e->Render();
		}
	}

	for (DebugRenderEntity* e : m_messageEntities)
	{
		if (e)
		{
			e->Render();
		}
	}
}

DebugRenderEntity::~DebugRenderEntity()
{
	delete m_vbo;
}

DebugRenderEntity::DebugRenderEntity(
	DebugRenderSystem& system
	, VertexBuffer* vbo
	, float duration
	, int count
	, Rgba8 startColor
	, Rgba8 endColor
	, DebugRenderMode mode
	, bool wireframe
	, BillboardType billboardType
)
	: m_system(system)
	, m_vbo(vbo)
	, m_timer(duration)
	, m_count(count)
	, m_startColor(startColor)
	, m_endColor(endColor)
	, m_mode(mode)
	, m_wireframe(wireframe)
	, m_isGarbage(false)
	, m_billboardType(billboardType)
{
}

void DebugRenderEntity::FixedUpdate()
{
	if (!m_timer.IsStopped() && m_timer.m_period >= 0.f)
	{
		if (m_timer.HasPeriodElapsed())
		{
			m_isGarbage = true;
		}
	}

	if (m_billboardType != BillboardType::NONE)
	{
		m_modelMatrix = GetBillboardMatrix(m_billboardType,
			m_system.m_target,
			m_modelMatrix.GetTranslation3D(),
			Vec2(1.f, 1.f)
		);
	}
}

void DebugRenderEntity::Render()
{
	Renderer& renderer = *m_system.GetConfig().m_renderer;
	float frac = m_timer.GetElapsedFraction();
	frac = GetClamped(frac,0.f,1.f);
	renderer.BindTexture(m_texture);
	if (m_wireframe)
	{
		renderer.SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
	}
	else
	{
		renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}

	if (m_mode == DebugRenderMode::X_RAY)
	{
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
		Rgba8 color = Rgba8::Mix(m_startColor, m_endColor, frac);
		renderer.SetModelConstants(m_modelMatrix, Rgba8(
			color.r,
			color.g,
			color.b,
			static_cast<unsigned char>((static_cast<float>(color.a) / 255.f) * 125)));
		renderer.DrawVertexBuffer(m_vbo, m_count, 0);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.SetModelConstants(m_modelMatrix, color);
		renderer.DrawVertexBuffer(m_vbo, m_count, 0);
	}
	else if (m_mode == DebugRenderMode::USE_DEPTH)
	{
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.SetModelConstants(m_modelMatrix, Rgba8::Mix(m_startColor, m_endColor, frac));
		renderer.DrawVertexBuffer(m_vbo, m_count, 0);
	}
	else if (m_mode == DebugRenderMode::ALWAYS)
	{
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
		renderer.SetModelConstants(m_modelMatrix, Rgba8::Mix(m_startColor, m_endColor, frac));
		renderer.DrawVertexBuffer(m_vbo, m_count, 0);
	}
}