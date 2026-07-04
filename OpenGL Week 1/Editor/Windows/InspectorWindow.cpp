#include "InspectorWindow.h"
#include <imgui.h>
#include <iostream>
#include "../InspectorSlotRegistry.h"
#include "../../ObjectSystem/ComponentFactory.h"
#include "../../Utils/Debug.h"
#include <gtc/type_ptr.hpp>
#include "../EditorWindowRegistry.h"

REGISTER_EDITOR_WINDOW(InspectorWindow);

void InspectorWindow::Draw(EditorContext& context)
{
    GameObject*& selected =  context.selectedGameObject;
    
    ImGui::Begin("Inspector");

    ImGui::TextWrapped("Drag a script asset (*.script) here to add it to this GameObject:");
    if (ImGui::BeginDragDropTarget())
    {
        if (auto pd = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD"))
        {
            int idx = *(int*)pd->Data;
            const auto& a = AssetManager::GetAssets()[idx];
            if (a.type == AssetType::Script && selected)
            {
                namespace fs = std::filesystem;
                std::string typeName = fs::path(a.path).stem().string();
                Component* comp = ComponentFactory::Instance()
                    .Create(typeName, nlohmann::json::object(), selected);
                if (!comp)
                    std::cerr << "[Inspector] No component for type: " << typeName << "\n";
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (!selected) {
        ImGui::Text("Nothing selected.");
        ImGui::End();
        return;
    }
    char buf[128];
    strncpy_s(buf, selected->name.c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    if (ImGui::InputText("##Rename", buf, sizeof(buf))) {
        selected->name = buf;
    }
    ImGui::SameLine();
    ImGui::Text("Name");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", glm::value_ptr(selected->transform.position), 0.1f);
        glm::vec3 euler = glm::degrees(glm::eulerAngles(selected->transform.rotation));
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.5f)) {
            selected->transform.rotation = glm::quat(glm::radians(euler));
        }
        ImGui::DragFloat3("Scale", glm::value_ptr(selected->transform.scale), 0.1f);
    }
    ImGui::Separator();

    for (int i = 0; i < (int)selected->components.size(); ++i) {
        Component* comp = selected->components[i].get();
        ImGui::PushID(comp);
        bool open = ImGui::CollapsingHeader(typeid(*comp).name(), ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Remove Component")) {
                selected->RemoveComponent(comp);
                ImGui::EndPopup();
                ImGui::PopID();
                break;
            }
            ImGui::EndPopup();
        }

        if (open) {
            for (auto& slot : InspectorSlotRegistry::GetSlotsFor(comp)) {
                ImGui::Text("%s", slot.label.c_str());
                ImGui::SameLine();
                ImGui::PushID(slot.label.c_str());
                std::string cur = slot.getter(comp);
                if (ImGui::Button(cur.empty() ? "<none>" : cur.c_str(), ImVec2(-1, 0))) {}
                if (ImGui::BeginDragDropTarget()) {
                    if (auto pd = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                        int idx2 = *(int*)pd->Data;
                        const Asset& a2 = AssetManager::GetAssets()[idx2];
                        if (std::find(slot.acceptedTypes.begin(), slot.acceptedTypes.end(), a2.type) != slot.acceptedTypes.end())
                            slot.setter(comp, a2);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
            ImGui::Separator();

            comp->OnInspectorGUI();
            ImGui::Separator();
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Component…")) ImGui::OpenPopup("AddComponentPopup");
    if (ImGui::BeginPopup("AddComponentPopup")) {
        for (auto& [typeName, entry] : ComponentFactory::Instance().GetRegistry()) {
            if (ImGui::MenuItem(typeName.c_str())) {
                ComponentFactory::Instance().Create(typeName, nlohmann::json::object(), selected);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}
const char* InspectorWindow::GetWindowName() const
{
    return "Inspector";
}