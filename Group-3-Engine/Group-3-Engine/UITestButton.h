#pragma once
#include "UIButton.h"

class UITestScene;

class UITestButton : public UIButton
{
public:
	using UIButton::UIButton;

	void OnClicked() override;
	//void OnHover() override;
	//void OnRelease() override;

};

