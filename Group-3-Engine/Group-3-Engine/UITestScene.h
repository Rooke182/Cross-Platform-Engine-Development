#pragma once
#include "UIScene.h"

class UITestScene : public UIScene
{
public:
	 void Init() override;
	 void Update(Time time) override;
	 void Render() override;
	 void Cleanup() override;
	 void EndFrame() override;

	 bool m_show;

	 void KeyNotify(int key, int action) override;
};

