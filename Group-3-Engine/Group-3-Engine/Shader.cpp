#include "Shader.h"
#include "Resources.h"

ShaderProgram Shader::GenerateShaderProgram(std::string vertexPath, std::string fragmentPath)
{
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	auto vertexSource = Resources::GetStringFromFile(vertexPath);
	auto fragmentSource = Resources::GetStringFromFile(fragmentPath);

	#ifdef OPENGL
	InitShader(vertexShader, GL_VERTEX_SHADER, vertexSource.c_str());
	InitShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentSource.c_str());

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
#ifdef DEBUGGER
		Debugger::AddError("Warning", "Error linking shader program: ", false);
#endif
		std::cout << infoLog << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_ERROR, "Shader Load Error", "%s", infoLog);
#endif
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
	#endif
	return 0;
}
