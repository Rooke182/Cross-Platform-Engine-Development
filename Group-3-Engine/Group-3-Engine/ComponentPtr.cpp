#include "ComponentPtr.h"
#include "Scene.h"

BaseComponentPtr::BaseComponentPtr(std::vector<BaseComponentPtr*>* container):
	m_container(container)
{
}

void BaseComponentPtr::UpdateContainer(std::vector<BaseComponentPtr*>* container)
{
	m_container = container;

	if(m_container != nullptr)
		m_container->push_back(this);
}

void to_json(nlohmann::json& json, const ComponentPtr<BaseComponent>& componentPtr)
{
	json["m_component"] = componentPtr->GetGuid();
}

void from_json(const nlohmann::json& json, ComponentPtr<BaseComponent>& componentPtr)
{
	componentPtr = Scene::GetCurrentScene()->GetComponentByGuid<BaseComponent>((Guid)json["m_component"]);
}