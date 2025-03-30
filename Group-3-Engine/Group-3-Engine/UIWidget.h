#pragma once
#include "Vector2.h"
#include "GameObject.h"
#include "SpriteRenderComponent.h"


	class UIScene;


	enum UIType
	{
		TEXT,
		BUTTON,
		IMAGE,
		DEFAULT
	};

	class UIWidget
	{
	public:
		explicit UIWidget(ComponentPtr<TransformComponent> transform, ComponentPtr<RenderComponent> renderer, UIScene* scene);

		explicit UIWidget(UIWidget* parent);
		UIWidget() {}

		virtual void Init();
		virtual void Render();
		virtual void Update(Time time);
		virtual void OnHover() {}
		virtual void OnClicked() {}
		virtual void OnRelease() {}
		void AddComponent(ComponentPtr<BaseComponent> component);
		void SetParent(UIWidget* parent) { m_parent = parent; }
		void SetType(UIType type) { m_type = type; }
		UIType GetType() { return m_type; }

		ComponentPtr<RenderComponent> GetRenderComponent() { return m_renderComponent; }
		ComponentPtr<TransformComponent> GetTransformComponent() { return m_transformComponent; }
		UIScene* GetScene() { return m_scene; }
		UIWidget* GetParent() { return m_parent; }

		template<typename T>
		ComponentPtr<T> GetComponentOfType();

	protected:
		ComponentPtr<RenderComponent> m_renderComponent;
		ComponentPtr<TransformComponent> m_transformComponent;
		std::vector<ComponentPtr<BaseComponent>> m_components;
		UIScene* m_scene;
		UIWidget* m_parent;
		UIType m_type = DEFAULT;
	};


	template<typename T>
	inline ComponentPtr<T> UIWidget::GetComponentOfType()
	{
		for (auto& component : m_components)
		{
			ComponentPtr<T> ptr = component.DynamicComponentCast<T>();
			if (ptr != nullptr)
				return ptr;
		}

		return nullptr;
	}
