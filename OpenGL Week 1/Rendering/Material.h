#pragma once

#include "../EnginePluginAPI.h"
#include "MaterialPropertyValue.h"
#include <glew.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>

class InspectorPropertyList;
class ENGINE_API Material
{
public:
    std::string name = "New Material";
    std::string path = "";

    std::string vertShaderPath = "Resources/Shaders/Texture.vert";
    std::string fragShaderPath = "Resources/Shaders/Texture.frag";

    std::unordered_map<std::string, MaterialPropertyValue> properties;

    bool isDirty = false;

    bool LoadFromFile(const std::string& filePath);
    bool SaveToFile(const std::string& filePath) const;

    nlohmann::json Serialize() const;
    void Deserialize(const nlohmann::json& json);

    void SyncPropertiesWithShader(GLuint shaderProgram);
    void ApplyPropertiesToShader(GLuint shaderProgram) const;
    void BuildInspectorProperties(
        InspectorPropertyList& propertyList,
        GLuint shaderProgram
    );

private:
    MaterialPropertyValue CreateDefaultPropertyValueFromShader(
    GLuint shaderProgram,
    const ShaderPropertyInfo& shaderProperty
) const;
};