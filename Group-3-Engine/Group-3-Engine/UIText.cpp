#include "UIText.h"
#include "TextRenderComponent.h"

UIText::UIText(ComponentPtr<TransformComponent> transform, ComponentPtr<TextRenderComponent> renderer, UIScene* scene)
{
	m_parent = nullptr;
	m_textRenderer = renderer;
	m_transformComponent = transform;
	m_scene = scene;
}

void UIText::Init()
{
	SetType(TEXT);

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

	AddComponent(m_textRenderer);
	AddComponent(m_transformComponent);
}
