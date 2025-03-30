#pragma once
#include "Texture.h"
class FontTexture : public Texture
{
protected:
	struct GlyphData
	{
		float m_advanceX;
		float m_advanceY;
		float m_bitmapWidth;
		float m_bitmapHeight;
		float m_bitmapLeft;
		float m_bitmapTop;
		float m_uvX;
		float m_uvY;
	};

	std::map<char, GlyphData> m_fontCharacters;

public:

	std::vector<Vertex> GenerateVertices(std::string text, float scale = 1.0f);
	void LoadFromFont(FT_Face face, int size = 64);
};

