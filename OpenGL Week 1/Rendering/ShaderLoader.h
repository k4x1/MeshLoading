#pragma once
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ShaderPropertyInfo.h"
#include "../EnginePluginAPI.h"

class ENGINE_API ShaderLoader
{
public:
    static GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    static GLuint CreateComputeProgram(const char* computeShaderFilename);
    
    static const std::vector<ShaderPropertyInfo>& GetMaterialProperties(GLuint program);
    
private:
    ShaderLoader(void);
    ~ShaderLoader(void);
    static GLuint CreateShader(GLenum shaderType, const char* shaderName);
    static std::string ReadShaderFile(const char* filename);
    static void PrintErrorDetails(bool isShader, GLuint id, const char* name);

    
    static void ReflectMaterialProperties(GLuint program);
    static bool ShouldExposeUniformAsMaterialProperty(const std::string& uniformName);
    static ShaderPropertyType ConvertOpenGLUniformType(GLenum uniformType,const std::string& uniformName);
    
    static std::unordered_map<GLuint, std::vector<ShaderPropertyInfo>> materialPropertiesByProgram;
    
};
