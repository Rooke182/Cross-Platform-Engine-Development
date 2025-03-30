#include "UITestQuitButton.h"
#include "Application.h"

void UITestQuitButton::OnClicked()
{
	UIButton::OnClicked();
	if (static_cast<UITestScene*>(m_scene))
	{
		UITestScene* scene = static_cast<UITestScene*>(m_scene);

		if (scene->m_show == true)
		{
			exit(0);
		}

	}
}
