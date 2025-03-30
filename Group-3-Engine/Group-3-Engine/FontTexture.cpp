#include "FontTexture.h"
std::vector<Vertex> FontTexture::GenerateVertices(std::string text, float scale)
{
	std::vector<Vertex> vertices;

	float x = 0.0f;
	float y = 0.0f;

	for (int i = 0; i < text.size(); i++)
	{
		if (m_fontCharacters.find(text[i]) != m_fontCharacters.end())
		{
			auto letter = text[i];
			GlyphData data = m_fontCharacters[text[i]];

			float vertX = x + data.m_bitmapLeft * scale;
			float vertY = -y - data.m_bitmapTop * scale;
			float w = data.m_bitmapWidth * scale;
			float h = data.m_bitmapHeight * scale;

			x += data.m_advanceX * scale;
			y += data.m_advanceY * scale;

			if (!w || !h)
				continue;

			vertices.push_back({ vertX, vertY, 0.0f, data.m_uvX, data.m_uvY });
			vertices.push_back({ vertX + w, vertY, 0.0f, data.m_uvX + (data.m_bitmapWidth / (float)m_width), data.m_uvY });
			vertices.push_back({ vertX, vertY + h, 0.0f, data.m_uvX, data.m_uvY + (data.m_bitmapHeight / (float)m_height) });

			vertices.push_back({ vertX + w, vertY, 0.0f, data.m_uvX + (data.m_bitmapWidth / (float)m_width), data.m_uvY });
			vertices.push_back({ vertX, vertY + h, 0.0f, data.m_uvX, data.m_uvY + (data.m_bitmapHeight / (float)m_height) });
			vertices.push_back({ vertX + w, vertY + h, 0.0f, data.m_uvX + (data.m_bitmapWidth / (float)m_width), data.m_uvY + (data.m_bitmapHeight / (float)m_height) });
		}
	}

	return vertices;
}

void FontTexture::LoadFromFont(FT_Face face, int size)
{
	FT_Set_Pixel_Sizes(face, 0, size);

	//first calculate the texture width/height
	int textureWidth = 0;
	int textureHeight = 0;
	int currentRowWidth = 0;
	int currentRowHeight = 0;

	for (int i = 32; i < 128; i++)
	{
		if (!FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			if (currentRowWidth + face->glyph->bitmap.width + 1 >= 1024) //new line
			{
				if (textureWidth < currentRowWidth)
					textureWidth = currentRowWidth; //update max width
				textureHeight += currentRowHeight;
				currentRowWidth = 0;
				currentRowHeight = 0;
			}

			currentRowWidth += face->glyph->bitmap.width + 1;
			if (currentRowHeight < (int)face->glyph->bitmap.rows) //update max row height
				currentRowHeight = face->glyph->bitmap.rows;

		}
	}
	if (textureWidth < currentRowWidth)
		textureWidth = currentRowWidth; //update max width
	textureHeight += currentRowHeight;

	//now generate texture
	#ifdef OPENGL
	glGenTextures(1, &this->m_textureID);
	glBindTexture(this->m_textureType, this->m_textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexParameteri(this->m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(this->m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	#endif

	this->m_width = textureWidth;
	this->m_height = textureHeight;

	#ifdef OPENGL
	glTexImage2D(this->m_textureType, 0, GL_RED, this->m_width, this->m_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glGenerateMipmap(this->m_textureType);
	#endif

	//now apply glyph textures to one big texture

	textureWidth = 0;
	textureHeight = 0;
	currentRowWidth = 0;
	currentRowHeight = 0;

	for (int i = 32; i < 128; i++)
	{
		if (!FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			if (currentRowWidth + face->glyph->bitmap.width + 1 >= 1024) //new line
			{
				if (textureWidth < currentRowWidth)
					textureWidth = currentRowWidth; //update max width
				textureHeight += currentRowHeight;
				currentRowWidth = 0;
				currentRowHeight = 0;
			}

			#ifdef OPENGL
			glTexSubImage2D(this->m_textureType, 0, currentRowWidth, textureHeight,
				face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			#endif

			this->m_fontCharacters[i].m_advanceX = (float)(face->glyph->advance.x >> 6);
			this->m_fontCharacters[i].m_advanceY = (float)(face->glyph->advance.y >> 6);

			this->m_fontCharacters[i].m_bitmapWidth = (float)face->glyph->bitmap.width;
			this->m_fontCharacters[i].m_bitmapHeight = (float)face->glyph->bitmap.rows;

			this->m_fontCharacters[i].m_bitmapLeft = (float)face->glyph->bitmap_left;
			this->m_fontCharacters[i].m_bitmapTop = (float)face->glyph->bitmap_top;

			this->m_fontCharacters[i].m_uvX = currentRowWidth / (float)this->m_width;
			this->m_fontCharacters[i].m_uvY = textureHeight / (float)this->m_height;

			currentRowWidth += face->glyph->bitmap.width + 1;
			if (currentRowHeight < (int)face->glyph->bitmap.rows) //update max row height
				currentRowHeight = face->glyph->bitmap.rows;
		}
	}
}
