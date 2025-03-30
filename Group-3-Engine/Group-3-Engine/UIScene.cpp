#include "UIScene.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"
#include "SpriteRenderComponent.h"
#include "Application.h"

	void UIScene::Init()
	{
		for (auto& widget : m_widgets)
		{
			widget->Init();
		}

		m_components.Iterate([](void* ptr)
		{
			BaseComponent* component = (BaseComponent*)ptr;
			component->Init();
		});
		
	}

	void UIScene::Update(Time time)
	{
		for (auto& widget : m_widgets)
		{
			widget->Update(time);
		}

		m_components.Iterate([time](void* ptr)
			{
				BaseComponent* component = (BaseComponent*)ptr;
				component->Update(time);
			});
	}

	void UIScene::Render()
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

	void UIScene::Cleanup()
	{
	}

	void UIScene::EndFrame()
	{
	
	}

	void UIScene::CursorNotify(double xPos, double yPos)
	{
		for (auto& widget : m_widgets)
		{
			if (widget->GetType() == BUTTON)
			{
				//convert transform to screenspace
				Vector2 screenCoords = WorldToScreen(widget->GetTransformComponent()->m_position);
				float width = widget->GetRenderComponent()->GetRenderData()->m_texture->m_width;
				float height = widget->GetRenderComponent()->GetRenderData()->m_texture->m_height;

				//button boundaries
				if (xPos> screenCoords.x && xPos < screenCoords.x + width)
				{
					if (yPos > screenCoords.y && yPos < screenCoords.y + height)
					{
						widget->OnHover();
					}
					else
					{
						widget->OnRelease();
					}
				}
				else
				{
					widget->OnRelease();
				}
			}
		}
	}


	void UIScene::MouseButtonNotify(int mouseButton, int action)
	{
#ifdef _WIN32
		if (action == GLFW_PRESS)
		{
			for (auto& widget : m_widgets)
			{
				if (widget->GetType() == BUTTON)
				{
					//convert transform to screenspace
					Vector2 screenCoords = WorldToScreen(widget->GetTransformComponent()->m_position);
					float width = widget->GetRenderComponent()->GetRenderData()->m_texture->m_width;
					float height = widget->GetRenderComponent()->GetRenderData()->m_texture->m_height;

					//button boundaries
					if (Application::m_mouseXpos > screenCoords.x && Application::m_mouseXpos < screenCoords.x + width)
					{
						if (Application::m_mouseYpos > screenCoords.y && Application::m_mouseYpos < screenCoords.y + height)
						{
							widget->OnClicked();
						}
					}
				}
			}
		}
		else if (action == GLFW_RELEASE)
		{
			OnRelease();
		}
#endif
	}

	Vector2 UIScene::WorldToScreen(glm::vec3 worldPos)
	{
		float halfWindowX = Application::m_windowWidth / 2;
		float halfWindowY = Application::m_windowHeight / 2;

		return Vector2(worldPos.x + halfWindowX, worldPos.y + halfWindowY);
	}

	UIWidget* UIScene::GetWidget(UIType type)
	{
		for (auto& widget : m_widgets)
		{
			if (widget->GetType() == type)
			{
				return widget;
			}
		}
	}

	void UIScene::OnClicked()
	{
		for (auto& widget : m_widgets)
		{
			if (widget->GetType() == BUTTON)
			{
				widget->OnClicked();
			}
		}
	}

	void UIScene::OnRelease()
	{
		for (auto& widget : m_widgets)
		{
			if (widget->GetType() == BUTTON)
			{
				widget->OnRelease();
			}
		}
	}

