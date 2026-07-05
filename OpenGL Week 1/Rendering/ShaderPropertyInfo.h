#pragma once

#include "../EnginePluginAPI.h"

#include <string>

enum class ENGINE_API ShaderPropertyType
{
    Float,
    Int,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    Color,
    Texture2D,
    Unknown
};

struct ENGINE_API ShaderPropertyInfo
{
    std::string name = "";
    ShaderPropertyType type = ShaderPropertyType::Unknown;
    int location = -1;
    int arraySize = 1;
};