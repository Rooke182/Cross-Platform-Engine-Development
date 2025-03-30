#pragma once
#include "Defines.h"
#include "RenderComponent.h"
#include "Resources.h"
#include "Vertex.h"
class Collider;

class ColliderRenderComponent : public RenderComponent
{
private: 
	ComponentPtr<Collider> m_collider;
public:
	virtual void Init() override;
	virtual void Render() override;
	void RenderComponentMenu(int ID) override;


	CreateComponentID(ColliderRenderComponent);

	std::string GetName() override { return "ColliderRenderComponent"; };
};

