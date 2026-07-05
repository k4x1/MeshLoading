#pragma once

#include "../EnginePluginAPI.h"
#include "ShaderPropertyInfo.h"

#include <glm.hpp>
#include <string>

struct ENGINE_API MaterialPropertyValue
{
    ShaderPropertyType type = ShaderPropertyType::Unknown;

    float floatValue = 0.0f;
    int intValue = 0;
    bool boolValue = false;

    glm::vec2 vec2Value = glm::vec2(0.0f);
    glm::vec3 vec3Value = glm::vec3(0.0f);
    glm::vec4 vec4Value = glm::vec4(1.0f);

    std::string texturePath = "";
};