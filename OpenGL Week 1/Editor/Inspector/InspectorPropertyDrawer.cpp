#include "InspectorPropertyDrawer.h"

#include "../../ObjectSystem/AssetManager.h"

#include <imgui.h>
#include <gtc/type_ptr.hpp>

#include <algorithm>
#include <cstring>

bool InspectorPropertyDrawer::DrawProperties(const InspectorPropertyList& propertyList)
{
    bool anyChanged = false;

    const std::vector<InspectorProperty>& properties = propertyList.GetProperties();

    for (const InspectorProperty& property : properties)
    {
        bool changed = DrawProperty(property);

        if (changed)
        {
            anyChanged = true;

            if (property.onChanged != nullptr)
            {
                property.onChanged();
            }
        }
    }

    return anyChanged;
}
bool InspectorPropertyDrawer::DrawProperty(const InspectorProperty& property)
{
    if (property.value == nullptr)
    {
        ImGui::Text("%s: <null>", property.label.c_str());
        return false;
    }

    if (property.type == InspectorPropertyType::Bool)
    {
        bool* value = static_cast<bool*>(property.value);
        return ImGui::Checkbox(property.label.c_str(), value);
    }

    if (property.type == InspectorPropertyType::Int)
    {
        int* value = static_cast<int*>(property.value);
        return ImGui::DragInt(property.label.c_str(), value);
    }

    if (property.type == InspectorPropertyType::Float)
    {
        float* value = static_cast<float*>(property.value);
        return ImGui::DragFloat(property.label.c_str(), value, property.dragSpeed);
    }

    if (property.type == InspectorPropertyType::String)
    {
        return DrawStringProperty(property);
    }

    if (property.type == InspectorPropertyType::Vec2)
    {
        glm::vec2* value = static_cast<glm::vec2*>(property.value);
        return ImGui::DragFloat2(property.label.c_str(), glm::value_ptr(*value), property.dragSpeed);
    }

    if (property.type == InspectorPropertyType::Vec3)
    {
        glm::vec3* value = static_cast<glm::vec3*>(property.value);
        return ImGui::DragFloat3(property.label.c_str(), glm::value_ptr(*value), property.dragSpeed);
    }

    if (property.type == InspectorPropertyType::Vec4)
    {
        glm::vec4* value = static_cast<glm::vec4*>(property.value);
        return ImGui::DragFloat4(property.label.c_str(), glm::value_ptr(*value), property.dragSpeed);
    }

    if (property.type == InspectorPropertyType::Color)
    {
        glm::vec4* value = static_cast<glm::vec4*>(property.value);
        return ImGui::ColorEdit4(property.label.c_str(), glm::value_ptr(*value));
    }

    if (property.type == InspectorPropertyType::AssetPath)
    {
        return DrawAssetPathProperty(property);
    }

    ImGui::Text("%s: Unsupported property type", property.label.c_str());
    return false;
}
bool InspectorPropertyDrawer::DrawStringProperty(const InspectorProperty& property)
{
    std::string* value = static_cast<std::string*>(property.value);

    char buffer[512];
    strncpy_s(buffer, value->c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText(property.label.c_str(), buffer, sizeof(buffer)))
    {
        *value = buffer;
        return true;
    }

    return false;
}
bool InspectorPropertyDrawer::DrawAssetPathProperty(const InspectorProperty& property)
{
    std::string* value = static_cast<std::string*>(property.value);

    ImGui::TextUnformatted(property.label.c_str());

    const char* buttonText = value->empty()
        ? "<none>"
        : value->c_str();

    ImGui::Button(
        buttonText,
        ImVec2(-1.0f, 0.0f)
    );

    bool changed = false;

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD");

        if (payload != nullptr)
        {
            int assetIndex = *static_cast<int*>(payload->Data);
            const std::vector<Asset>& assets = AssetManager::GetAssets();

            if (assetIndex >= 0 && assetIndex < static_cast<int>(assets.size()))
            {
                const Asset& asset = assets[assetIndex];

                bool accepted = false;

                for (AssetType acceptedType : property.acceptedAssetTypes)
                {
                    if (asset.type == acceptedType)
                    {
                        accepted = true;
                        break;
                    }
                }

                if (accepted)
                {
                    *value = asset.path;
                    changed = true;
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Clear"))
        {
            value->clear();
            changed = true;
        }

        ImGui::EndPopup();
    }

    return changed;
}