#include "Timer.h"

void Timer::Start(float time, int loopCount, std::function<void()> callback)
{
	m_finishTime = time;
	m_loops = loopCount;
	m_callback = callback;

	m_started = true;
	m_finished = false; //start
}

void Timer::Stop()
{
	m_finished = true;
}

void Timer::Pause()
{
	m_paused = true;
}

void Timer::Resume()
{
	m_paused = false;
}

void Timer::Finish()
{
	if (m_callback)
		m_callback();

	m_timePassed = 0.0f; //reset

	if (m_loops > 0) //if loop count is 0 then just repeat forever
	{
		m_loops--;
		if (m_loops <= 0) //now finished when loops are done
			m_finished = true;
	}
}

void Timer::Update(Time time)
{
	if (!m_finished && !m_paused)
	{
		m_timePassed += time.m_deltaTime;

		if (m_timePassed >= m_finishTime)
		{
			Finish();
		}
	}
}

void Timer::Cleanup()
{
	Stop();
}
