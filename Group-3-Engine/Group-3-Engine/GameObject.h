#pragma once
#include <vector>
#include <json.hpp>
#include "BaseComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CameraComponent.h"
#include "Time.h"
#include "ComponentPtr.h"
#include "Guid.h"
#include <algorithm>

class Scene;

class GameObject
{
private:
	std::vector<ComponentPtr<BaseComponent>> m_components;
	std::vector<GameObject*> m_children;
	GameObject* m_parent;
	ComponentPtr<TransformComponent> m_transform;
	ComponentPtr<RenderComponent> m_renderer;
	Scene* m_scene;
	Guid m_guid;

	std::string m_name = "Game Object";

public:
	GameObject();
	GameObject(const GameObject& other);
	//rvalue reference to show that the list of components can ONLY belong to the gameobject, and can't be shared
	GameObject(std::vector<ComponentPtr<BaseComponent>>&& components, 
		ComponentPtr<TransformComponent> transform, ComponentPtr<RenderComponent> renderer, Scene* scene) noexcept;

	GameObject(GameObject&& gameObject) noexcept;

	void Setup();

	void AddChild(GameObject* child);
	void SetParent(GameObject* parent);
	void AddComponent(ComponentPtr<BaseComponent> component);
	void RemoveComponent(ComponentPtr<BaseComponent> component);
	void AddComponentAtIndex(ComponentPtr<BaseComponent> component, unsigned int index);

	GameObject* GetParent() const { return m_parent; }
	ComponentPtr<TransformComponent> GetTransform() const { return m_transform; }
	ComponentPtr<RenderComponent> GetRenderer() const { return m_renderer; }
	Scene* GetScene() const { return m_scene; }
	const Guid& GetGuid() const { return m_guid; }
	std::string GetName() const { return m_name; }

	void SetTransform(ComponentPtr<TransformComponent> transform);
	void SetRenderer(ComponentPtr<RenderComponent> renderer);
	void SetGuid(const Guid& guid);
	void SetName(const char* name);

	template<typename T>
	ComponentPtr<T> GetComponentOfType() const;

	template<typename T>
	std::vector<ComponentPtr<T>> GetComponentsOfType() const;

	std::vector<Guid> GetChildrenGuids() const;

	

	GameObject& operator=(const GameObject& other);
	
	friend class Scene;
	friend class Debugger;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;
};

template<typename T>
inline ComponentPtr<T> GameObject::GetComponentOfType() const
{
	for (auto& component : m_components)
	{
        ComponentPtr<T> ptr = component.DynamicComponentCast<T>();
		if (ptr != nullptr)
			return ptr;
	}

	return nullptr;
}

template<typename T>
inline std::vector<ComponentPtr<T>> GameObject::GetComponentsOfType() const
{
	std::vector<ComponentPtr<T>> componentsOfType;

	for (auto& component : m_components)
	{
        ComponentPtr<T> ptr = component.DynamicComponentCast<T>();
		if (ptr != nullptr)
			componentsOfType.push_back(ptr);
	}

	return componentsOfType;
}

void to_json(nlohmann::json& json, const GameObject& component);
void from_json(const nlohmann::json& json, GameObject& component);