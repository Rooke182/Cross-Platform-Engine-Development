#pragma once
#include "PackedBuffer.h"
#include "ComponentPtr.h"
#include <map>

template <int SIZE>
class ComponentBuffer : public PackedBuffer<SIZE>
{
private:
	std::map<void*, std::vector<BaseComponentPtr*>> m_componentPointers;
	//for each address of a component, it stores a vector of pointers to the ComponentPointers given out
	//That way, if the address of a component is changed, the ComponentPointers can be updated

public:
	template <typename T, typename... Args>
	ComponentPtr<T> CreateComponent(Args... args);
	void Deallocate(void* ptr) override;

	template<typename T>
	ComponentPtr<T> GetComponentOfType();

	template<typename T>
	std::vector<ComponentPtr<T>> GetComponentsOfType();

	template<typename T>
	ComponentPtr<T> GetComponentByGuid(const Guid& guid);
};

template<int SIZE>
inline void ComponentBuffer<SIZE>::Deallocate(void* ptr)
{
	//Remove deallocated component from the map and set its ComponentPointers to nullptr
	for (auto& componentPtr : m_componentPointers[ptr])
	{
		componentPtr->UpdatePtr(nullptr);
		componentPtr->UpdateContainer(nullptr);
	}
	m_componentPointers.erase(ptr);
	PackedBuffer<SIZE>::Deallocate(ptr);
}

template<int SIZE>
template<typename T, typename... Args>
inline ComponentPtr<T> ComponentBuffer<SIZE>::CreateComponent(Args... args)
{
	void* componentVoid = PackedBuffer<SIZE>::Allocate(sizeof(T), 16);

	T* component = new (componentVoid) T(args...);
	m_componentPointers.emplace(componentVoid, std::vector<BaseComponentPtr*>{});
	ComponentPtr<T> comptr = ComponentPtr<T>(component, &m_componentPointers[componentVoid]);

	return comptr;
}

template<int SIZE>
template<typename T>
inline ComponentPtr<T> ComponentBuffer<SIZE>::GetComponentOfType()
{
	T* component = nullptr;
	ComponentPtr<T> componentPtr;

	PackedBuffer<SIZE>::Iterate([&component, &componentPtr, this](void* ptr)
		{
			if (component == nullptr)
			{
				component = dynamic_cast<T*>((BaseComponent*)ptr);
				if (component != nullptr)
					componentPtr = ComponentPtr<T>(component, &m_componentPointers[component]);
			}
			
		});

	return componentPtr;
}

template<int SIZE>
template<typename T>
inline std::vector<ComponentPtr<T>> ComponentBuffer<SIZE>::GetComponentsOfType()
{
	std::vector<ComponentPtr<T>> components;

    PackedBuffer<SIZE>::Iterate([&components, this](void* ptr)
	{
		T* component = dynamic_cast<T*>((BaseComponent*)ptr);
		if (component != nullptr)
			components.push_back(ComponentPtr<T>(component, &m_componentPointers[component]));

	});

	return components;
}

template<int SIZE>
template<typename T>
inline ComponentPtr<T> ComponentBuffer<SIZE>::GetComponentByGuid(const Guid& guid)
{
	ComponentPtr<T> componentPtr;

	PackedBuffer<SIZE>::Iterate([&componentPtr, this, guid](void* ptr)
	{
		T* component = (T*)ptr;
		if (component->GetGuid() == guid)
		{
			componentPtr = ComponentPtr<T>(component, &m_componentPointers[component]);
		}
	});

	return componentPtr;
}
