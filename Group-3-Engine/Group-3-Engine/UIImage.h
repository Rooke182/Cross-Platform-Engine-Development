#pragma once
#include "UIWidget.h"
#include "SpriteRenderComponent.h"



	class UIImage : public UIWidget
	{
		using UIWidget::UIWidget;

		void SetTexture(std::string filePath) { m_renderComponent->m_texturePath = filePath; }
		void Init() override;

		ComponentPtr<SpriteRenderComponent> m_renderComponent;
	};
