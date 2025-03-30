#pragma once
#include "RenderComponent.h"
#include "GameObject.h"
#include "Resources.h"
#include "FontTexture.h"
class TextRenderComponent : public RenderComponent
{
public:
	virtual void Init() override;
	virtual void Cleanup() override;

	const std::string GetText() { return m_text; }
	const int GetFontSize() { return m_fontSize; }
	void SetText(std::string t);
	void UpdateText();
	void SetFont(std::string f);
	void SetFontSize(unsigned int s);

	CreateComponentID(TextRenderComponent);
	SerialiseVariables(3, m_text, m_font, m_fontSize);
	std::string GetName() override { return "TextRenderComponent"; };
private:

	std::string m_text = "the cat";
	std::string m_font = "assets/fonts/comic.ttf";
	bool m_dirtyText = false;
	unsigned int m_fontSize = 32;
	std::shared_ptr<FontTexture> m_fontTexture = nullptr;
	FT_Face m_face;

	static FT_Library ft;
	static bool startedFT;
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
	void RenderComponentMenu(int ID) override;

};

