#include "Texture.h"
#include "Resources.h"

void Texture::LoadFromFile(std::string path, int format, bool flipY)
{
	m_path = path;
    path = Resources::FormatPath(path);
	stbi_set_flip_vertically_on_load(flipY);
#ifdef ANDROID
    AAsset* asset = AAssetManager_open(Resources::GetAssetManager(), path.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "File Load Error", "%s", path.c_str());
        return;
    }
    unsigned char* charData;
    size_t size = (size_t)AAsset_getLength(asset);
    charData = (unsigned char*)malloc(sizeof(unsigned char)*size);
    AAsset_read(asset, charData, size);
    AAsset_close(asset);
    unsigned char* data = stbi_load_from_memory(charData, size, &this->m_width, &this->m_height, &this->m_channels, 4);
#else
    unsigned char* data = stbi_load(path.c_str(), &this->m_width, &this->m_height, &this->m_channels, 4);
#endif

	if (!data)
	{
		Debug::Warning("problem loading texture from " + path, false);
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_ERROR, "Texture Load Error", "%s", path.c_str());
#endif
		stbi_image_free(data);
		return;
	}

	#ifdef OPENGL
		glGenTextures(1, &this->m_textureID);
		glBindTexture(this->m_textureType, this->m_textureID);

		glTexParameteri(this->m_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(this->m_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(this->m_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(this->m_textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(this->m_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(this->m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	#endif

	#ifdef OPENGL
	if (format == 0) //automatic
	{
		switch (this->m_channels)
		{
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
		}
	}
	glTexImage2D(this->m_textureType, 0, GL_RGBA, this->m_width, this->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(this->m_textureType);
	#endif


	stbi_image_free(data);
}
