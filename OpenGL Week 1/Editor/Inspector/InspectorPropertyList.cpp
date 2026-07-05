#include "InspectorPropertyList.h"

void InspectorPropertyList::Clear()
{
    properties.clear();
}

void InspectorPropertyList::AddBool(
    const std::string& label,
    bool* value,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Bool;
    property.value = value;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddInt(
    const std::string& label,
    int* value,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Int;
    property.value = value;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddFloat(
    const std::string& label,
    float* value,
    float dragSpeed,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Float;
    property.value = value;
    property.dragSpeed = dragSpeed;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddString(
    const std::string& label,
    std::string* value,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::String;
    property.value = value;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddVec2(
    const std::string& label,
    glm::vec2* value,
    float dragSpeed,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Vec2;
    property.value = value;
    property.dragSpeed = dragSpeed;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddVec3(
    const std::string& label,
    glm::vec3* value,
    float dragSpeed,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Vec3;
    property.value = value;
    property.dragSpeed = dragSpeed;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddVec4(
    const std::string& label,
    glm::vec4* value,
    float dragSpeed,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Vec4;
    property.value = value;
    property.dragSpeed = dragSpeed;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddColor(
    const std::string& label,
    glm::vec4* value,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::Color;
    property.value = value;
    property.onChanged = onChanged;

    properties.push_back(property);
}

void InspectorPropertyList::AddAssetPath(
    const std::string& label,
    std::string* value,
    const std::vector<AssetType>& acceptedAssetTypes,
    const std::function<void()>& onChanged
)
{
    InspectorProperty property;
    property.label = label;
    property.type = InspectorPropertyType::AssetPath;
    property.value = value;
    property.acceptedAssetTypes = acceptedAssetTypes;
    property.onChanged = onChanged;

    properties.push_back(property);
}

const std::vector<InspectorProperty>& InspectorPropertyList::GetProperties() const
{
    return properties;
}