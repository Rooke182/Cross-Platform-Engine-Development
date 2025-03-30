#include "UITestScene.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UIText.h"
#include "UITestButton.h"
#include "UITestQuitButton.h"

void UITestScene::Init()
{
	auto camera = MakeComponent<CameraComponent>();

	//Buttons
	UITestQuitButton* quitBtn = new UITestQuitButton(MakeComponent<TransformComponent>(), MakeComponent<SpriteRenderComponent>("assets/textures/testUI/smallButton.png"), this);
	UITestButton* playBtn = new UITestButton(MakeComponent<TransformComponent>(), MakeComponent<SpriteRenderComponent>("assets/textures/testUI/smallButton.png"), this);
	m_widgets.push_back(quitBtn);
	m_widgets.push_back(playBtn);

	//Background
	UIImage* img = new UIImage(MakeComponent<TransformComponent>(), MakeComponent<SpriteRenderComponent>("assets/textures/testUI/background.jpeg"), this);
	m_widgets.push_back(img);

	//Title text
	UIText* titleTxt = new UIText(MakeComponent<TransformComponent>(), MakeComponent<TextRenderComponent>(), this);
	m_widgets.push_back(titleTxt);

	UIScene::Init();


	playBtn->GetTransformComponent()->m_position = { -50.0f, -150.0f, 1000.0f };
	auto playTxt = playBtn->GetButtonTextWidget();
	playTxt->GetTextRenderComponent()->SetText("Play");

	quitBtn->GetTransformComponent()->m_position = { -50.0f, 0.0f, 1000.0f };
	auto quitTxt = quitBtn->GetButtonTextWidget();
	quitTxt->GetTextRenderComponent()->SetText("Quit"); 

	img->GetTransformComponent()->m_position = { -640.f, -360.f, 500.0f };
	titleTxt->GetTransformComponent()->m_position = { -50.0f, -250.0f, 1000.0f };
	titleTxt->GetTextRenderComponent()->SetText("Menu");
	m_show = true;
}

void UITestScene::Update(Time time)
{
	UIScene::Update(time);
}

void UITestScene::Render()
{
	if (m_show)
	{
		UIScene::Render();
	}
}

void UITestScene::Cleanup()
{
}

void UITestScene::EndFrame()
{
}

void UITestScene::KeyNotify(int key, int action)
{
#ifdef _WIN32
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_P)
		{
			m_show = true;
		}
	}
#endif
}
