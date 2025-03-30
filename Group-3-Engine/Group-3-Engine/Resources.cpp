#include "Resources.h"
#include "Shader.h"
#include "Texture.h"
#include "FontTexture.h"

std::map<std::string, std::shared_ptr<Texture>> Resources::m_storedTextures;
std::map<std::string, std::shared_ptr<FontTexture>> Resources::m_storedFonts;
std::map<std::string, ShaderProgram> Resources::m_storedShaderPrograms;

#ifdef ANDROID
AAssetManager* Resources::m_androidAssetManager = nullptr;
#endif

std::string Resources::FormatPath(std::string path)
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
		{
			path[i] = '/';
		}
	}
#ifdef ANDROID
	//need to remove assets prefix for android version	
	path = path.substr(path.find("assets/") + 7);
	//std::cout << formattedPath << std::endl;
#endif

	return path;
}

ShaderProgram Resources::GetShaderProgram(std::string vertexPath, std::string fragmentPath)
{
	auto key = vertexPath + fragmentPath;
	if (m_storedShaderPrograms.find(key) == m_storedShaderPrograms.end()) //doesnt exist so create it
	{
#ifdef ANDROID
		unsigned int value = Shader::GenerateShaderProgram(vertexPath + "-android.vert", fragmentPath + "-android.frag");
#else
        unsigned int value = Shader::GenerateShaderProgram(vertexPath + ".vert", fragmentPath + ".frag");
#endif
		m_storedShaderPrograms[key] = value;
	}

	return m_storedShaderPrograms[key];
}

std::shared_ptr<Texture> Resources::GetTexture(std::string key, int format)
{
	if (m_storedTextures.find(key) == m_storedTextures.end()) //doesnt exist so create it
	{
		m_storedTextures[key] = std::make_shared<Texture>();
		m_storedTextures[key]->LoadFromFile(key, format);
	}
	return m_storedTextures[key];
}

std::shared_ptr<FontTexture> Resources::GetFontTexture(std::string path, FT_Face face, int size)
{
	std::string key = path + std::to_string(size);
	if (m_storedFonts.find(key) == m_storedFonts.end()) //doesnt exist so create it
	{
		m_storedFonts[key] = std::make_shared<FontTexture>();
		m_storedFonts[key]->LoadFromFont(face, size);
	}
	return m_storedFonts[key];
}

void Resources::ClearShaderPrograms()
{
	for (auto& shader : m_storedShaderPrograms)
	{
		#ifdef OPENGL
		glDeleteProgram(shader.second);
		#endif
	}
	m_storedShaderPrograms.clear();
}

void Resources::ClearTextures()
{	
	m_storedTextures.clear();
}

void Resources::ClearFontTextures()
{
	m_storedFonts.clear();
}

void Resources::ClearAll()
{
	ClearShaderPrograms();
	ClearTextures();
	ClearFontTextures();
}

void Resources::ClearUnusedTextures()
{

	/*for (auto tex : m_storedTextures)
	{
		std::cout << tex.first << std::endl;
		std::cout << tex.second.use_count() << std::endl;
	}
	for (auto tex : m_storedFonts)
	{
		std::cout << tex.first << std::endl;
		std::cout << tex.second.use_count() << std::endl;
	}
	std::cout << std::endl;*/
	for (auto i = m_storedTextures.cbegin(); i != m_storedTextures.cend();)
	{
		if (i->second.use_count() <= 2) //unused??
			m_storedTextures.erase(i++);
		else
			++i;
	}
	for (auto i = m_storedFonts.cbegin(); i != m_storedFonts.cend();)
	{
		if (i->second.use_count() <= 2) //unused??
			m_storedFonts.erase(i++);
		else
			++i;
	}
}