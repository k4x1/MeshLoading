#pragma once
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "EnginePluginAPI.h"

class ENGINE_API ShaderLoader
{
public:
    static GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    static GLuint CreateComputeProgram(const char* computeShaderFilename);

private:
    ShaderLoader(void);
    ~ShaderLoader(void);
    static GLuint CreateShader(GLenum shaderType, const char* shaderName);
    static std::string ReadShaderFile(const char* filename);
    static void PrintErrorDetails(bool isShader, GLuint id, const char* name);
};
