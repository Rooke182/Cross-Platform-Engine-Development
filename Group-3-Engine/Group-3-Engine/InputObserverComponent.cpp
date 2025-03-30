#include "InputObserverComponent.h"
#include "Scene.h"
#include "Input.h"
#include "GameObject.h"

void InputObserverComponent::Init()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->AddObserver(this, "InputObserverComponent");
}

void InputObserverComponent::Cleanup()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->RemoveObserver(this);
}
