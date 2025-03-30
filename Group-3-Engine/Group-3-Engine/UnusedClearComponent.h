#pragma once
#include "BaseComponent.h"
class UnusedClearComponent : public BaseComponent
{
public:
	virtual void Init();

	CreateComponentID(UnusedClearComponent);


	std::string GetName() override { return "UnusedClearComponent"; };
};

