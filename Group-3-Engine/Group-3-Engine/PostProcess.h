#pragma once
#include "Resources.h"
class Application;

struct IPostProcessValue
{
	std::string m_uniformName;
	virtual void ApplyValueToShader(ShaderProgram shaderProgram) {};
};

struct PostProcess
{
	std::string name;
	unsigned int m_frameBuffer;
	unsigned int m_depthBuffer;
	unsigned int m_texture;
	ShaderProgram m_shaderProgram;
	std::vector<IPostProcessValue*> m_values;

	void Cleanup()
	{
		glDeleteFramebuffers(1, &m_frameBuffer);
		glDeleteBuffers(1, &m_depthBuffer);
		glDeleteTextures(1, &m_texture);
		for (int i = 0; i < m_values.size(); i++)
		{
			delete m_values[i];
		}
	}

	void Init();
	void Resize(int width, int height);
};



template <typename T>
struct PostProcessValue : IPostProcessValue
{
	T m_value;
	PostProcessValue(T value, std::string name) { m_value = value; m_uniformName = name; }
};

struct PostProcessFloat : PostProcessValue<float>
{
	PostProcessFloat(float value, std::string name) : PostProcessValue<float>(value, name) {};
	virtual void ApplyValueToShader(ShaderProgram shaderProgram) override
	{
		glUniform1f(glGetUniformLocation(shaderProgram, m_uniformName.c_str()), m_value);
	}
};

struct PostProcessFloat4 : PostProcessValue<glm::vec4>
{
	PostProcessFloat4(glm::vec4 value, std::string name) : PostProcessValue<glm::vec4>(value, name) {};
	virtual void ApplyValueToShader(ShaderProgram shaderProgram) override
	{
		glUniform4f(glGetUniformLocation(shaderProgram, m_uniformName.c_str()), m_value.x, m_value.y, m_value.z, m_value.w);
	}
};

