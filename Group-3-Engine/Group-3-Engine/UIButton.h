#pragma once
#include "UIWidget.h"
#include "UIText.h"


	class UIButton : public UIWidget
	{
	public:
		using UIWidget::UIWidget;

		virtual void Init() override;
		virtual void Update(Time time) override;
		virtual void OnClicked() override;
		virtual void OnHover() override;
		virtual void OnRelease() override;

		UIText* GetButtonTextWidget() { return m_text; }

	private:
		UIText* m_text;
	};