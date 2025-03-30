#include "UIWidget.h"
#include "UIScene.h"


	UIWidget::UIWidget(
		ComponentPtr<TransformComponent> transform, ComponentPtr<RenderComponent> renderer, UIScene* scene) : 
	m_parent(nullptr), m_renderComponent(renderer), m_transformComponent(transform), m_scene(scene)
	{
	}

	UIWidget::UIWidget(UIWidget* parent) : m_parent(parent), m_scene(parent->m_scene)
	{
		m_renderComponent = m_scene->MakeComponent<RenderComponent>();
		m_transformComponent = m_scene->MakeComponent<TransformComponent>();
	}


	void UIWidget::Init()
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

		for (auto& component : m_components)
		{
			component->SetUIParent(this);
		}

		AddComponent(m_renderComponent);
		AddComponent(m_transformComponent);
	}

	void UIWidget::Render()
	{
		m_renderComponent->Render();
	}

	void UIWidget::Update(Time time)
	{
	}

	void UIWidget::AddComponent(ComponentPtr<BaseComponent> component)
	{
		if (!component.IsNull())
		{
			component->SetUIParent(this);
			m_components.push_back(component);
		}
	}
