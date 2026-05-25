#include "Timer.hpp"
#include "Engine/Core/Clock.hpp"

Timer::Timer(float period, Clock const* clock /*= nullptr*/)
{
	if (clock == nullptr)
	{
		m_clock = &Clock::GetSystemClock();
	}
	else
	{
		m_clock = clock;
	}
	m_startTime = m_clock->GetTotalSeconds() + 0.000000001;
	m_period = period;
}

void Timer::Start()
{
	m_startTime = m_clock->GetTotalSeconds() + 0.000000000001;
}

void Timer::Stop()
{
	m_startTime = 0.;
}

float Timer::GetElapsedTime() const
{
	if (m_startTime == 0.)
	{
		return 0.;
	}
	double totalTime = m_clock->GetTotalSeconds();
	double timeDisplaced = totalTime - m_startTime;
	return static_cast<float>(timeDisplaced);
}

float Timer::GetElapsedFraction() const
{
	return static_cast<float>(GetElapsedTime() / m_period);
}

bool Timer::IsStopped() const
{
	return m_startTime == 0.;
}

bool Timer::HasPeriodElapsed() const
{
	return static_cast<float>(GetElapsedTime() > m_period);
}

bool Timer::DecrementPeriodIfElapsed()
{
	if (HasPeriodElapsed())
	{
		m_startTime += m_period;
		return true;
	}
	return false;
}

