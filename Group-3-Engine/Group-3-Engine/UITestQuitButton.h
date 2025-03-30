#pragma once
#include "UIButton.h"

class UITestQuitButton : public UIButton
{
	using UIButton::UIButton;

	void OnClicked() override;
};

