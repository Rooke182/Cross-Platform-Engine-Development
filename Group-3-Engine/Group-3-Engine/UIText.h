#pragma once
#include "UIWidget.h"
#include "TextRenderComponent.h"


	class UIText : public UIWidget
	{	
	public:
		explicit UIText(ComponentPtr<TransformComponent> transform, ComponentPtr<TextRenderComponent> renderer, UIScene* scene);

		void Init() override;
		ComponentPtr<TextRenderComponent> GetTextRenderComponent() { return m_textRenderer; }

		ComponentPtr<TextRenderComponent> m_textRenderer;
	};

