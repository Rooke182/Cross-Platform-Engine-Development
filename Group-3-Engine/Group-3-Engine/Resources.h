#pragma once
#include "Defines.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "RenderData.h"
#ifdef ANDROID
#include <android/asset_manager_jni.h>
#endif
class Shader;
class Texture;
class FontTexture;

class Resources
{
private: 

	static std::map<std::string, std::shared_ptr<Texture>> m_storedTextures;
	static std::map<std::string, std::shared_ptr<FontTexture>> m_storedFonts;
	static std::map<std::string, ShaderProgram> m_storedShaderPrograms;
#ifdef ANDROID
    static AAssetManager* m_androidAssetManager;
#endif

public:

#ifdef ANDROID
    static AAssetManager* GetAssetManager() { return m_androidAssetManager; }
    static void SetAssetManager(AAssetManager* manager) { m_androidAssetManager = manager; }
#endif

	static std::string FormatPath(std::string path);

	static std::string GetStringFromFile(std::string path)
	{
        path = FormatPath(path);
#ifdef ANDROID
        std::string data = "";
        AAsset* asset = AAssetManager_open(m_androidAssetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        if (asset == NULL)
        {
            __android_log_print(ANDROID_LOG_ERROR, "File Load Error", "%s", path.c_str());
            return data;
        }
        char* charData;
        size_t size = (size_t)AAsset_getLength(asset);
        charData = (char*)malloc(sizeof(char)*size);
        AAsset_read(asset, charData, size);
        AAsset_close(asset);
        data.assign(charData);
        data.resize(size);
        return data;
#else
		std::ifstream file(path);
		std::string data = "";
		if (file)
		{
			std::ostringstream stringStream;
			stringStream << file.rdbuf();
			data = stringStream.str();
		}
		else
		{
#ifdef DEBUGGER
			Debugger::AddError("Warning", "Failed to load string from path: " + path, false);
#endif
		}

		return data;
#endif
	}

	static ShaderProgram GetShaderProgram(std::string vertexPath, std::string fragmentPath);
	static std::shared_ptr<Texture> GetTexture(std::string path, int format = 0);
	static std::shared_ptr<FontTexture> GetFontTexture(std::string path, FT_Face fact, int size);


	static void ClearShaderPrograms();
	static void ClearTextures();
	static void ClearFontTextures();
	static void ClearAll();
	static void ClearUnusedTextures();
};

