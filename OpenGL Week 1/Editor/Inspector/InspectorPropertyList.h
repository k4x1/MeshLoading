#pragma once

#include "../../EnginePluginAPI.h"
#include "InspectorProperty.h"

#include <glm.hpp>

#include <string>
#include <vector>

class ENGINE_API InspectorPropertyList
{
public:
    void Clear();

    void AddBool(
        const std::string& label,
        bool* value,
        const std::function<void()>& onChanged = nullptr
    );

    void AddInt(
        const std::string& label,
        int* value,
        const std::function<void()>& onChanged = nullptr
    );

    void AddFloat(
        const std::string& label,
        float* value,
        float dragSpeed = 0.01f,
        const std::function<void()>& onChanged = nullptr
    );

    void AddString(
        const std::string& label,
        std::string* value,
        const std::function<void()>& onChanged = nullptr
    );

    void AddVec2(
        const std::string& label,
        glm::vec2* value,
        float dragSpeed = 0.01f,
        const std::function<void()>& onChanged = nullptr
    );

    void AddVec3(
        const std::string& label,
        glm::vec3* value,
        float dragSpeed = 0.01f,
        const std::function<void()>& onChanged = nullptr
    );

    void AddVec4(
        const std::string& label,
        glm::vec4* value,
        float dragSpeed = 0.01f,
        const std::function<void()>& onChanged = nullptr
    );

    void AddColor(
        const std::string& label,
        glm::vec4* value,
        const std::function<void()>& onChanged = nullptr
    );

    void AddAssetPath(
        const std::string& label,
        std::string* value,
        const std::vector<AssetType>& acceptedAssetTypes,
        const std::function<void()>& onChanged = nullptr
    );

    const std::vector<InspectorProperty>& GetProperties() const;

private:
    std::vector<InspectorProperty> properties;
};