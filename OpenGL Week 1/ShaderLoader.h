#pragma once

// Library Includes
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <string>
#include <vector>

class ShaderLoader
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
