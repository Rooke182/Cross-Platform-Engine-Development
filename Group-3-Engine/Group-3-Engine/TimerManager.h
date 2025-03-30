#pragma once
#include "Timer.h"
#include "BaseComponent.h"
#include <vector>
class TimerManager : public BaseComponent
{
public: 
	CreateComponentID(TimerManager);

	virtual Timer* GetNewTimer();
	virtual void Update(Time time) override;
	virtual void Cleanup() override;
private: 
	std::vector<Timer*> m_timers;
};

