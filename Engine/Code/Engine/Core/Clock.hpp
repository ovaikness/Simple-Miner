#pragma once
#include <vector>

class Clock
{
public:
	Clock();
	explicit Clock(Clock& parent);

	//Unparents self and children but otherwise does nothing. Fixing the hierarchy is up to the user
	~Clock();
	Clock(Clock const& copy) = delete;

	void Reset();

	bool IsPaused() const;
	void Pause();
	void Unpause();
	void TogglePause();

	void StepSingleFrame();

	void SetTimeScale(float timeScale);
	float  GetTimeScale() const;

	float  GetDeltaSeconds() const;
	double GetTotalSeconds() const;
	int GetFrameCount() const;

public:
	static Clock& GetSystemClock();

	static void TickSystemClock();

protected:
	void Tick();
	void Advance(double deltaTimeSeconds);
	void AddChild(Clock* childClock);
	void RemoveChild(Clock* childclock);

protected:
	Clock* m_parent = nullptr;

	std::vector<Clock*> m_children;

	double m_lastUpdateTimeInSeconds = 0.;
	double m_totalSeconds = 0.;
	double m_deltaSeconds = 0.;
	int m_frameCount = 0;

	double m_timeScale = 1.;

	bool m_isPaused = false;

	bool m_stepSingleFrame = false;

	double m_maxDeltaSeconds = 0.1;
};