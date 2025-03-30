#include "RotatorComponent.h"
#include "GameObject.h"

void RotatorComponent::Update(Time time)
{
	m_parent->GetTransform()->m_angle += m_DeltaRotation * time.m_deltaTime;
}
