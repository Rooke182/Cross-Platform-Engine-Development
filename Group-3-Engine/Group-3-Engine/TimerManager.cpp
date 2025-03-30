#include "TimerManager.h"

Timer* TimerManager::GetNewTimer()
{
	for (Timer* timer : m_timers)
	{
		if (timer->m_finished) //find first timer that is already finished
			return timer;
	}

	Timer* timer = new Timer();
	m_timers.push_back(timer);
	return timer;
}

void TimerManager::Update(Time time)
{
	for (auto& timer : m_timers)
		timer->Update(time);
}

void TimerManager::Cleanup()
{
	for (auto& timer : m_timers)
	{
		timer->Cleanup();
		delete timer;
	}
	m_timers.clear();
}
