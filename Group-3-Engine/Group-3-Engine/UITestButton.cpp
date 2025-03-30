#include "UITestButton.h"
#include "UITestScene.h"

void UITestButton::OnClicked()
{
	UIButton::OnClicked();

	if (static_cast<UITestScene*>(m_scene))
	{
		UITestScene* scene = static_cast<UITestScene*>(m_scene);

		scene->m_show = false;
	}
}
