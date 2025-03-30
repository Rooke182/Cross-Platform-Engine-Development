#pragma once
#include "Time.h"
#include <functional>
class Timer
{
public:
	virtual void Start(float time, int loopCount, std::function<void()> callback);
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	virtual void Finish();
	virtual void Update(Time time);
	virtual void Cleanup();
private: 
	bool m_paused = false;
	bool m_started = false;
	bool m_finished = true;
	int m_loops = 1;
	float m_timePassed = 0.0f;
	float m_finishTime = 0.0f;
	std::function<void()> m_callback;
	friend class TimerManager;

};

