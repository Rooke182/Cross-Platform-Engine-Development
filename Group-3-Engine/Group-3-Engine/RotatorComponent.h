#pragma once
#include "BaseComponent.h"
#include "ComponentPtr.h"
#include "TransformComponent.h"

class RotatorComponent : public BaseComponent
{
public:
	glm::vec3 m_DeltaRotation;

	CreateComponentID(RotatorComponent);

	virtual void Update(Time time);

	std::string GetName() override { return "RotatorComponent"; };
};

