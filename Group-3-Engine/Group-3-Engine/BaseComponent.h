#pragma once
#include <stdlib.h>
#include <cstdio>
#include "Time.h"
#include "Debugger.h"
#include "CompileTimeHash.h"
#include "Guid.h"
#include "SerialisationMacros.h"
#include "GlmSerialise.h"
#include <json.hpp>

#define GetComponentID(name) APHash(#name, sizeof(#name))

#define CreateComponentID(name)	virtual unsigned int GetId()	const	\
{																		\
	return GetComponentID(name);										\
}																		\
																		\
virtual void PublicSerialise(nlohmann::json& json)const;				\
virtual void PublicDeserialise(const nlohmann::json& component);		\

//Evaluate input, THEN stringify from: https://stackoverflow.com/a/50593836
#define Stringify_X(x) #x
#define Stringify(x) Stringify_X(x)

#define ToJson(x) json[Stringify(x)] = x;
#define FromJson(x) x = component[Stringify(x)];

#define SerialiseVariables(numArguments, ...) void Serialise(nlohmann::json& json) const override	\
{																									\
	BaseComponent::Serialise(json);																	\
	REPEAT_FUNC(ToJson, numArguments, __VA_ARGS__)													\
}																									\
																									\
void Deserialise(const nlohmann::json& component)													\
{																									\
	BaseComponent::Deserialise(component);															\
	REPEAT_FUNC(FromJson, numArguments, __VA_ARGS__)												\
}																									\

class GameObject;
class UIWidget;

class BaseComponent
{
protected:
	GameObject* m_parent = nullptr;
	UIWidget* m_UIparent = nullptr;
	bool m_MarkedForDeletion = false;
	Guid m_guid;

public:
    virtual ~BaseComponent() {};
	//deleting the regular new and using inplace since 
	//Scene will just give us a pointer to its buffer that we can use
	void* operator new(size_t size) = delete;
	void* operator new(size_t size, void* ptr);

	void SetParent(GameObject* parent);
	void SetUIParent(UIWidget* parent);
	GameObject* GetParent();
	UIWidget* GetUIParent();

	virtual void Init() {};
	virtual void Update(Time time) {};
	virtual void PreRender() {};
	virtual void Render() {};
	virtual void PostRender() {};
	virtual void Cleanup() {};
	virtual std::string GetName() { return "BaseComponent"; };

	virtual void Serialise(nlohmann::json& json)const;
	virtual void Deserialise(const nlohmann::json& component);
	virtual void PublicSerialise(nlohmann::json& json)const = 0;
	virtual void PublicDeserialise(const nlohmann::json& component) = 0;

	virtual unsigned int GetId() const = 0;

	void SetGuid(const Guid& guid);
	Guid& GetGuid();
	virtual void RenderComponentMenu(int ID) 
	{
#ifdef DEBUGGER
		std::string label = this->GetName() + "##" + std::to_string(ID);
		if (ImGui::CollapsingHeader(label.c_str()))
		{
			RemoveComponentMenu(ID);
		}
#endif
	};
	virtual void RemoveComponentMenu(int ID);

	friend class Debugger;
	friend class DebuggerMenus;
	friend class Scene;
	friend class DebuggerFunctions;

	virtual void AddComponent() {};

	void DeleteComponent();
	bool IsMarkedForDelete();

};

void to_json(nlohmann::json& json, const BaseComponent& component);
void from_json(const nlohmann::json& json, BaseComponent& component);
