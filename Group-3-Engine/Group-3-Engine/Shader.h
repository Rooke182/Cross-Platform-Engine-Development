#pragma once
#include "Defines.h"
#include <string>
#include "Resources.h"
#include "Debug.h"
#include "Debugger.h"

class Resources;

class Shader
{
public:

	static ShaderProgram GenerateShaderProgram(std::string vertexPath, std::string fragmentPath);

	static void InitShader(unsigned int& shader, unsigned int type, const char* source)
	{
#ifdef OPENGL
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
#ifdef DEBUGGER
			Debugger::AddError("Warning", "Error Compiling shader: ", false);
#endif
			std::cout << infoLog << std::endl;
#ifdef ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "Shader Load Error", "%s", infoLog);
#endif
		}
#endif
	}
};

