#pragma once

class Clock;

class Timer
{
public:
	explicit Timer(float period, Clock const* clock = nullptr);

	void Start();

	void Stop();

	float GetElapsedTime() const;

	float GetElapsedFraction() const;

	bool IsStopped() const;

	bool HasPeriodElapsed() const;

	bool DecrementPeriodIfElapsed();

	Clock const* m_clock = nullptr;

	double m_startTime = 0.;
	double m_period = 0.;
};