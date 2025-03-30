#include "BaseComponent.h"
#include "UIWidget.h"

void* BaseComponent::operator new(size_t size, void* ptr)
{
	return ptr;
}

void BaseComponent::SetParent(GameObject* parent)
{
	m_parent = parent;
}

GameObject* BaseComponent::GetParent()
{
	return m_parent;
}

void BaseComponent::Serialise(nlohmann::json& json) const
{
	json["id"] = GetId();
	json["m_guid"] = m_guid;

	PublicSerialise(json);
}

void BaseComponent::Deserialise(const nlohmann::json& component)
{
	m_guid = Guid(component["m_guid"]);
	PublicDeserialise(component);
}

void BaseComponent::SetGuid(const Guid& guid)
{
	m_guid = guid;
}

Guid& BaseComponent::GetGuid()
{
	return m_guid;
}

void BaseComponent::RemoveComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string lable = "Remove Component##" + std::to_string(ID);
	if (DebuggerFunctions::Button(lable.c_str()))
	{
		this->DeleteComponent();
	}
#endif
}

void BaseComponent::SetUIParent(UIWidget* parent)
{
	m_UIparent = parent;
}

UIWidget* BaseComponent::GetUIParent()
{
	return m_UIparent;
}

void BaseComponent::DeleteComponent()
{
	m_MarkedForDeletion = true;
}

bool BaseComponent::IsMarkedForDelete()
{
	return m_MarkedForDeletion;
}

void to_json(nlohmann::json& json, const BaseComponent& component)
{
	component.Serialise(json);
}

void from_json(const nlohmann::json& json, BaseComponent& component)
{
	component.Deserialise(json);
}
