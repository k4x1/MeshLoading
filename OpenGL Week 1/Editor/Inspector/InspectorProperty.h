#pragma once

#include "../../EnginePluginAPI.h"
#include "../../ObjectSystem/AssetManager.h"

#include <glm.hpp>

#include <functional>
#include <string>
#include <vector>

enum class ENGINE_API InspectorPropertyType
{
    Unknown,
    Bool,
    Int,
    Float,
    String,
    Vec2,
    Vec3,
    Vec4,
    Color,
    AssetPath
};

struct ENGINE_API InspectorProperty
{
    std::string label = "";
    InspectorPropertyType type = InspectorPropertyType::Unknown;

    void* value = nullptr;

    float dragSpeed = 0.01f;
    float minValue = 0.0f;
    float maxValue = 0.0f;
    bool useRange = false;

    std::vector<AssetType> acceptedAssetTypes;

    std::function<void()> onChanged;
};