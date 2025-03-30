#include "UnusedClearComponent.h"
#include "GameObject.h"
#include "TimerManager.h"
#include "Resources.h"

void UnusedClearComponent::Init()
{
	auto timerManager = m_parent->GetComponentOfType<TimerManager>();

	auto testTimer = timerManager->GetNewTimer();
	testTimer->Start(5000, 0, [=]()
		{
			Resources::ClearUnusedTextures();
			//Debug::Warning("test timer loop");
		});
}
