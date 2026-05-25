#include "Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

Clock s_systemClock = Clock();

Clock::Clock()
{
	if (this != &s_systemClock)
	{
		s_systemClock.AddChild(this);
		m_parent = &s_systemClock;
		m_lastUpdateTimeInSeconds = m_parent->GetTotalSeconds();
		m_totalSeconds = m_parent->GetTotalSeconds();
	}
	else
	{
		m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
	};
}

Clock::Clock(Clock& parent)
	: m_parent(&parent)
{
	parent.AddChild(this);
}

void Clock::Reset()
{
	m_lastUpdateTimeInSeconds = 0.;
	m_totalSeconds = 0.;
	m_deltaSeconds = 0.;
}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Unpause()
{
	m_isPaused = false;
}

void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

void Clock::StepSingleFrame()
{
	m_lastUpdateTimeInSeconds = m_totalSeconds;
	double deltaSeconds = 0.01666666666666666666666666666667;
	deltaSeconds = std::min(0.1, deltaSeconds);
	Advance(deltaSeconds);
}

void Clock::SetTimeScale(float timeScale)
{
	m_timeScale = timeScale;
}

float Clock::GetTimeScale() const
{
	return static_cast<float>(m_timeScale);
}

float Clock::GetDeltaSeconds() const
{
	return static_cast<float>(m_deltaSeconds);
}

double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

int Clock::GetFrameCount() const
{
	return m_frameCount;
}

Clock& Clock::GetSystemClock()
{
	return s_systemClock;
}

void Clock::TickSystemClock()
{
	s_systemClock.Tick();
}

void Clock::Tick()
{
	if (m_isPaused)
	{
		m_lastUpdateTimeInSeconds = m_totalSeconds;
		m_deltaSeconds = 0.;
		for (Clock*& clock : m_children)
		{
			if (clock)
			{
				clock->m_deltaSeconds = 0.f;
			}
		}
		return;
	}
	
	if (this == &s_systemClock)
	{
		m_totalSeconds = GetCurrentTimeSeconds();
		m_deltaSeconds = MinFloat(static_cast<float>(m_totalSeconds - m_lastUpdateTimeInSeconds), 0.1f) * m_timeScale;
		m_lastUpdateTimeInSeconds = m_totalSeconds;
		for (Clock*& clock : m_children)
		{
			if (clock)
			{
				clock->Tick();
			}
		}
	}
	else
	{
		double deltaSeconds = m_parent->GetDeltaSeconds() * m_timeScale;
		Advance(deltaSeconds);
	}
}

void Clock::Advance(double deltaTimeSeconds)
{
	m_deltaSeconds = deltaTimeSeconds;
	m_lastUpdateTimeInSeconds = m_totalSeconds;
	m_totalSeconds += m_deltaSeconds;

	for (Clock*& clock : m_children)
	{
		if (clock)
		{
			clock->Tick();
		}
	}
}

void Clock::AddChild(Clock* childClock)
{
	for (Clock*& indexedChild : m_children)
	{
		if (indexedChild == childClock)
		{
			return;
		}
	}

	for (Clock*& indexedChild : m_children)
	{
		if (indexedChild == nullptr)
		{
			indexedChild = childClock;
			return;
		}
	}

	m_children.push_back(childClock);
}

void Clock::RemoveChild(Clock* childClock)
{
	for(int i = 0; i < m_children.size(); ++ i)
	{
		if (m_children[i] == childClock)
		{
			m_children[i] = nullptr;
		}
	}
}

Clock::~Clock()
{
	if (m_parent)
	{
		m_parent->RemoveChild(this);
	}
	for (Clock*& clock : m_children)
	{
		if (clock)
		{
			clock->m_parent = m_parent;
		}
	}
}
