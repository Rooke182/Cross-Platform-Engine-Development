#pragma once
#include <vector>
#include <assert.h>
#include <json.hpp>
#include "BaseComponent.h"

//Base type of all ComponentPtrs
//Done this way since UpdatePtr wants to access m_component
//But also we want to store our ComponentPtrs consecutively
class BaseComponentPtr
{
protected:
	std::vector<BaseComponentPtr*>* m_container;
	//Container contains pointers to all the other ComponentPtrs for a given component

public:
	BaseComponentPtr(std::vector<BaseComponentPtr*>* container);
	void UpdateContainer(std::vector<BaseComponentPtr*>* container);
	virtual void UpdatePtr(void* ptr) {};
};

template <typename T>
class ComponentPtr : public BaseComponentPtr
{
private:
	T* m_component;

public:
	ComponentPtr();
	ComponentPtr(T* component);
	ComponentPtr(T* component, std::vector<BaseComponentPtr*>* container);
	ComponentPtr(const ComponentPtr<T>& other);
	ComponentPtr(ComponentPtr<T>& other);

	~ComponentPtr();

	virtual void UpdatePtr(void* ptr) override;

	bool IsNull();

	T* operator->() const;

	//These two functions let us implicitly convert from ComponentPtr<T> -> ComponentPtr<Type> if that conversion can be done on the base types
	operator ComponentPtr<BaseComponent>();	
	template <typename Type> operator ComponentPtr<Type>();

	ComponentPtr<T>& operator=(const ComponentPtr<T>& other);
	bool operator==(const ComponentPtr<T>& other);
	bool operator!=(const ComponentPtr<T>& other);

    template <typename Type>
    ComponentPtr<Type> DynamicComponentCast() const;
};

void to_json(nlohmann::json& json, const ComponentPtr<BaseComponent>& componentPtr);
void from_json(const nlohmann::json& json, ComponentPtr<BaseComponent>& componentPtr);


template<typename T>
inline ComponentPtr<T>::ComponentPtr():
	m_component(nullptr), BaseComponentPtr(nullptr)
{
}

template<typename T>
inline ComponentPtr<T>::ComponentPtr(T* component) :
	m_component(component), BaseComponentPtr(nullptr)
{
}

template<typename T>
inline ComponentPtr<T>::ComponentPtr(T* component, std::vector<BaseComponentPtr*>* container):
	m_component(component), BaseComponentPtr(container)
{
	if (m_container != nullptr)
	{
		m_container->push_back(this);
	}
}

template<typename T>
inline ComponentPtr<T>::ComponentPtr(const ComponentPtr<T>& other) :
	m_component(other.m_component), BaseComponentPtr(other.m_container)
{
	if (m_container != nullptr)
	{
		m_container->push_back(this);
	}
}

template<typename T>
inline ComponentPtr<T>::ComponentPtr(ComponentPtr<T>& other) :
	m_component(other.m_component), BaseComponentPtr(other.m_container)
{
	if (m_container != nullptr)
	{
		m_container->push_back(this);
	}
}

template<typename T>
inline ComponentPtr<T>::~ComponentPtr()
{
	if (m_container)
	{
		for (int i = 0; i < m_container->size(); i++)
		{
			if (m_container->at(i) == this)
			{
				m_container->erase(m_container->begin() + i);
				break;
			}
		}
	}

	m_component = nullptr;
	m_container = nullptr;
}

template<typename T>
inline void ComponentPtr<T>::UpdatePtr(void* ptr)
{
	m_component = (T*)ptr;
}

template<typename T>
inline bool ComponentPtr<T>::IsNull()
{
	return m_component == nullptr;
}

template<typename T>
inline T* ComponentPtr<T>::operator->() const
{
#ifdef _DEBUG
	//assert(m_component != nullptr);
#endif


	return m_component;
}

template<typename T>
inline ComponentPtr<T>& ComponentPtr<T>::operator=(const ComponentPtr<T>& other)
{
	if (this == &other)
		return *this;

	//remove component from old container
	if (m_container)
	{
		for (int i = 0; i < m_container->size(); i++)
		{
			if (m_container->at(i) == this)
			{
				m_container->erase(m_container->begin() + i);
			}
		}
	}

	m_component = other.m_component;
	m_container = other.m_container;

	if(m_container)
		m_container->push_back(this);
	return *this;
}

template<typename T>
inline bool ComponentPtr<T>::operator==(const ComponentPtr<T>& other)
{
	return m_component == other.m_component;
}

template<typename T>
inline bool ComponentPtr<T>::operator!=(const ComponentPtr<T>& other)
{
	return m_component != other.m_component;
}

template<typename T>
inline ComponentPtr<T>::operator ComponentPtr<BaseComponent>()
{
	return ComponentPtr<BaseComponent>(m_component, m_container);
}

template<typename T>
template<typename Type>
inline ComponentPtr<T>::operator ComponentPtr<Type>()
{
	return ComponentPtr<Type>(m_component, m_container);
}

template<typename T>
template<typename Type>
inline ComponentPtr<Type> ComponentPtr<T>::DynamicComponentCast() const
{
    return ComponentPtr<Type>(dynamic_cast<Type*>(m_component), m_container);
}