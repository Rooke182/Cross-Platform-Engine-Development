#include "Scene.h"
#include "Input.h"
#include "Resources.h"
#include "PreBuild.h"

Scene* Scene::m_currentScene;

Scene::Scene()
{
	m_gameObjects.reserve(256);
}

void Scene::Init()
{
	for (auto& gameobject : m_gameObjects)
	{
		gameobject.Setup();
	}

	m_components.Iterate([](void* ptr) 
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->Init();
	});
}

void Scene::Update(Time time)
{
	m_components.Iterate([time](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		if(component->GetParent() != nullptr)
		component->Update(time);
	});
	//Debugger::m_gameobjectCount = m_gameObjects.size();
}

void Scene::Render()
{
	m_components.Iterate([](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->PreRender();
	});

	m_components.Iterate([](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->Render();
	});

	m_components.Iterate([](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->PostRender();
	});
}

void Scene::Cleanup()
{
	m_components.Iterate([&](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->Cleanup();
		component->~BaseComponent();
		m_components.Deallocate(component);
	});

}

void Scene::EndFrame()
{
	std::vector<BaseComponent*> componentsToDelete;

	//want to delete anything marked for deletion
	m_components.Iterate([&componentsToDelete](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		if (component->IsMarkedForDelete())
			componentsToDelete.push_back(component);
	});

	for (auto& component : componentsToDelete)
	{
		//remove component from gameobject
		GameObject* gameobject = component->GetParent();

		for (int i = 0; i < gameobject->m_components.size(); i++)
		{
			if (gameobject->m_components[i] == component)
			{
				gameobject->m_components.erase(gameobject->m_components.begin() + i);
				break;
			}
		}

		//delete component
		component->Cleanup();
		component->~BaseComponent();
		m_components.Deallocate(component);
	}
}

void Scene::SetInput(Input* input)
{
	m_input = input;
}

Input* Scene::GetInput()
{
	return m_input;
}

int Scene::AddGameObject(GameObject&& gameObject)
{
	m_gameObjects.push_back(std::move(gameObject));
	return (int)m_gameObjects.size() - 1;
}

void Scene::IterateComponents(std::function<void(void*)> func)
{
	m_components.Iterate(func);
}

void Scene::IterateGameObjects(std::function<void(GameObject&)> func)
{
	for (auto& gameObject : m_gameObjects)
		func(gameObject);
}

nlohmann::json Scene::SerialiseScene()
{
	nlohmann::json scene;
	nlohmann::json sceneComponents;

	m_components.Iterate([&sceneComponents](void* ptr)
	{
		nlohmann::json componentJson;
		BaseComponent* component = (BaseComponent*)ptr;
		if (component->GetParent() == nullptr || component->IsMarkedForDelete()) {  Debug::Error("fuck" + std::string(component->GetName())); return;}
		component->Serialise(componentJson);

		sceneComponents.push_back(componentJson);
	});

	scene["components"] = sceneComponents;
	scene["gameobjects"] = m_gameObjects;

	return scene;
}

void Scene::DeserialiseScene(const nlohmann::json& json)
{
	//Do a prepass where we create components without deserialising
	//So that way, we can do a lookup for guids when actually deserialising
	for (const auto& component : json["components"])
	{
		int id = component["id"];
		Guid guid = component["m_guid"];
		auto createdComponent = CreateComponent(*this, id);
		createdComponent->SetGuid(guid);
	}

	int i = 0;
	m_components.Iterate([&i, json](void* ptr)
	{
		BaseComponent* component = (BaseComponent*)ptr;
		component->Deserialise(json["components"][i]);
		i++;
	});

	//Same for gameobjects,
	//Create blanks with correct guids so any lookups will work
	for (const auto& gameobject : json["gameobjects"])
	{
		GameObject blankObject;
		blankObject.m_guid = (Guid)gameobject["m_guid"];
		m_gameObjects.push_back(blankObject);
	}

	for (int i = 0; i < json["gameobjects"].size(); i++)
	{
		m_gameObjects[i] = json["gameobjects"][i];
		m_gameObjects[i].m_scene = this;
		if (json["gameobjects"][i].contains("m_name"))
		{
			std::string str = (std::string)json["gameobjects"][i]["m_name"];
			m_gameObjects[i].SetName(str.c_str());
		}
	}
}

GameObject* Scene::GetGameObjectByGuid(const Guid& guid)
{
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].GetGuid() == guid)
			return &m_gameObjects[i];
	}

	return nullptr;
}

GameObject* Scene::GetGameObjectByName(std::string name)
{
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].GetName() == name)
			return &m_gameObjects[i];
	}

	return nullptr;
}

std::vector<GameObject*> Scene::GetGameObjectsByName(std::string name)
{
	std::vector<GameObject*> list;
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].GetName() == name)
			list.push_back(& m_gameObjects[i]);
	}
	return list;
}

Scene* Scene::GetCurrentScene()
{
	return m_currentScene;
}

Scene* Scene::LoadFromJson(std::string filePath)
{
	if (m_currentScene)
	{
		delete m_currentScene;
		m_currentScene = nullptr;
	}

	m_currentScene = new Scene();

    std::string stringData = Resources::GetStringFromFile(filePath);
    if (stringData != "")
    {
        nlohmann::json json = nlohmann::json::parse(stringData);
        m_currentScene->DeserialiseScene(json);
    }

	
	return m_currentScene;
}

void Scene::WriteToJson(std::string filePath)
{
	std::ofstream output(filePath);
	output << std::setw(4) << SerialiseScene() << std::endl;
}
