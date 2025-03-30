#include "UIButton.h"
#include "SpriteRenderComponent.h"
#include "UIScene.h"



	void UIButton::Init()
	{
        UIWidget::Init();

		m_text = new UIText(m_scene->MakeComponent<TransformComponent>(), m_scene->MakeComponent<TextRenderComponent>(), m_scene);
		m_text->Init();

		SetType(BUTTON);
	}

	void UIButton::Update(Time time)
	{
		if (m_renderComponent.IsNull())
		{
			return;
		}

		const RenderData* renderData = m_renderComponent->GetRenderData();

		if (renderData == nullptr)
		{
			return;
		}

		const Texture* texture = renderData->m_texture.get();
		if(texture == nullptr)
		{
			return;
		}


		float length  = m_text->GetTextRenderComponent()->GetText().size();
		
		float offsetX = (texture->m_width / 2) - (length * (m_text->GetTextRenderComponent()->GetFontSize() / 2));
		float offsetY = (texture->m_height / 2) + 15;

		if (m_transformComponent->m_position.x < 0)
		{
			m_text->GetTransformComponent()->m_position = glm::vec3((m_transformComponent->m_position.x) + offsetX, (m_transformComponent->m_position.y) + offsetY, m_transformComponent->m_position.z + 50.0f);
		}
		else
		{
			m_text->GetTransformComponent()->m_position = glm::vec3((m_transformComponent->m_position.x / 2) + offsetX, (m_transformComponent->m_position.y) + offsetY, m_transformComponent->m_position.z + 50.0f);
		}
	}

	void UIButton::OnHover()
	{
		//Grey
		m_renderComponent->SetColor(glm::vec4(1, 1, 1, 0.5));
	}

	void UIButton::OnRelease()
	{
		//White
		m_renderComponent->SetColor(glm::vec4(1, 1, 1, 1));
	}

	void UIButton::OnClicked()
	{
		//Yellow
		m_renderComponent->SetColor(glm::vec4(1, 1, 0, 1));
	}

	 