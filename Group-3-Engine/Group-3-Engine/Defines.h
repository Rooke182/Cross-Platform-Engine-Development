#pragma once

#define NOMINMAX

#ifdef _WIN32
	#define OPENGL
    #include <glad/glad.h>
    //#define INPUTLOG
#endif

#ifdef ANDROID
    #define OPENGL
    #include <GLES3/gl3.h>
    #include <stdlib.h>
    #include<android/log.h>
    #define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "tag", __VA_ARGS__)
    #define DEBUGGER
#endif


#ifdef OPENGL
typedef unsigned int ShaderProgram;

#ifdef _DEBUG
#define DEBUGGER
#endif

#else
typedef unsigned int ShaderProgram;
#endif