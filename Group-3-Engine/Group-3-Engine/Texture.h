#pragma once
#include "Defines.h"
#include "Debug.h"
#ifdef ANDROID
#include <android/asset_manager_jni.h>
#endif
#include "Debugger.h"
#include <stb_image.h>
#include "Vertex.h"
#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

class Texture
{
public:
	unsigned int m_textureID = 0;
	#ifdef OPENGL
	int m_textureType = GL_TEXTURE_2D;
	#endif

	//for extra textures within shader
	int m_textureNum = 0;
	std::string m_textureShaderName = "texture0";
	std::string m_path;

	int m_width = 0, m_height = 0, m_channels = 0;

	void LoadFromFile(std::string path, int format = 0, bool flipY = false);

	void BindToShader(unsigned int shader)
	{
		#ifdef OPENGL
		glUniform1i(glGetUniformLocation(shader, m_textureShaderName.c_str()), m_textureNum);
		glActiveTexture(GL_TEXTURE0 + m_textureNum);
		glBindTexture(m_textureType, m_textureID);
		#endif
	}

	~Texture() { Cleanup(); }

	void Cleanup()
	{
		#ifdef OPENGL
		glDeleteTextures(1, &m_textureID);
		#endif	
	}
};