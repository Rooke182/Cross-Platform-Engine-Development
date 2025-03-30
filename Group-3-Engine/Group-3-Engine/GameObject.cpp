#include "GameObject.h"
#include "Scene.h"

GameObject::GameObject():
	m_components(), m_children(), m_parent(nullptr), m_transform(nullptr), m_renderer(nullptr), m_scene(nullptr)
{
}

GameObject::GameObject(const GameObject& other)
{
	m_components = other.m_components;
	m_children = other.m_children;
	m_parent = other.m_parent;
	m_transform = other.m_transform;
	m_renderer = other.m_renderer;
	m_scene = other.m_scene;
	m_guid = other.m_guid;
}

GameObject::GameObject(std::vector<ComponentPtr<BaseComponent>>&& components,
	ComponentPtr<TransformComponent> transform, ComponentPtr<RenderComponent> renderer, Scene* scene) noexcept:
	m_components(std::move(components)), m_children(), m_parent(nullptr), m_transform(transform), m_renderer(renderer), m_scene(scene)
{
	//Remove any null components from m_components
	for (int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i].IsNull())
		{
			m_components.erase(m_components.begin() + i);
			i--;
		}
	}
}

GameObject::GameObject(GameObject&& gameObject) noexcept :
	m_parent(gameObject.m_parent), m_transform(gameObject.m_transform), m_renderer(gameObject.m_renderer), m_scene(gameObject.m_scene)
{
	m_components = std::move(gameObject.m_components);
	m_children = std::move(gameObject.m_children);
}

void GameObject::Setup()
{
	//Since our gameobject gets constructed & then moved,
	//We want to wait till init for the address to settle
	//If we copy constructed, it would also have a different address, so same problem
	//We could instead have the scene store pointers, but then iterating over them would be less cache friendly
	for (auto& component : m_components)
		component->SetParent(this);

	if (m_transform != nullptr)
	{
		//AddComponent already does nullchecks & sets parent, so we can blindly add these
		AddComponent(m_transform);
	}
	else
	{
		auto transform = GetComponentOfType<TransformComponent>();
		if (transform != nullptr)
			SetTransform(transform);
	}
	
	if (m_renderer != nullptr)
	{
		//AddComponent already does nullchecks & sets parent, so we can blindly add these
		AddComponent(m_renderer);
	}
	else
	{
		auto renderer = GetComponentOfType<RenderComponent>();
		if (renderer != nullptr)
			SetRenderer(renderer);
	}
}

void GameObject::AddChild(GameObject* child)
{
	if (child)
	{
		m_children.push_back(child);
		child->SetParent(this);
	}
}

void GameObject::SetParent(GameObject* parent)
{
	m_parent = parent;
}

void GameObject::AddComponent(ComponentPtr<BaseComponent> component)
{
	if (!component.IsNull())
	{
		component->SetParent(this);
		m_components.push_back(component);
	}
}


void GameObject::RemoveComponent(ComponentPtr<BaseComponent> component)
{
	//component;
}

void GameObject::AddComponentAtIndex(ComponentPtr<BaseComponent> component, unsigned int index)
{
	if (!component.IsNull())
	{
		component->SetParent(this);
		m_components.insert(m_components.begin() + index, component);
	}
}

void GameObject::SetTransform(ComponentPtr<TransformComponent> transform)
{
	m_transform = transform;
}

void GameObject::SetRenderer(ComponentPtr<RenderComponent> renderer)
{
	m_renderer = renderer;
}

void GameObject::SetGuid(const Guid& guid)
{
	m_guid = guid;
}

void GameObject::SetName(const char* name)
{
	m_name = name;
}

std::vector<Guid> GameObject::GetChildrenGuids() const
{
	std::vector<Guid> guids = std::vector<Guid>(m_children.size());

	for (const auto& child : m_children)
	{
		guids.push_back(child->GetGuid());
	}

	return guids;
}

GameObject& GameObject::operator=(const GameObject& other)
{
	m_components = std::move(other.m_components);
	m_children = std::move(other.m_children);
	m_parent = other.m_parent;
	m_transform = other.m_transform;
	m_renderer = other.m_renderer;
	m_scene = other.m_scene;
	m_guid = other.m_guid;

	return *this;
}



void to_json(nlohmann::json& json, const GameObject& gameObject)
{
	json["m_guid"] = gameObject.GetGuid();
	json["m_components"] = gameObject.GetComponentsOfType<BaseComponent>();
	//Debug::Error("crap" + gameObject.GetComponentOfType<BaseComponent>()->GetGuid().ToString() + " " + gameObject.GetComponentOfType<BaseComponent>()->GetName() + " " + std::to_string(gameObject.GetComponentOfType<BaseComponent>()->GetId()));
	json["m_children"] = gameObject.GetChildrenGuids();

	json["m_name"] = gameObject.GetName();
	
	auto parent = gameObject.GetParent();
	if (parent)
		json["m_parent"] = gameObject.GetParent()->GetGuid();
	else
		json["m_parent"] = Guid::GetZeroGuid();
}

void from_json(const nlohmann::json& json, GameObject& gameObject)
{
	Guid guid = json["m_guid"];
	gameObject.SetGuid(guid);

	if (json.contains("m_name"))
	{
		std::string str = (std::string)json["m_name"];
		gameObject.SetName(str.c_str());
	}
		


	for (const auto& component : json["m_components"])
	{
		Guid componentGuid = component["m_component"];
		gameObject.AddComponent(Scene::GetCurrentScene()->GetComponentByGuid<BaseComponent>(componentGuid));
	}

	for (const auto& child : json["m_children"])
	{
		Guid childGuid = child["guid"];
		gameObject.AddChild(Scene::GetCurrentScene()->GetGameObjectByGuid(childGuid));
	}

	if ((Guid)json["m_parent"] != Guid::GetZeroGuid())
	{
		gameObject.SetParent(Scene::GetCurrentScene()->GetGameObjectByGuid((Guid)json["m_parent"]));
	}
}
