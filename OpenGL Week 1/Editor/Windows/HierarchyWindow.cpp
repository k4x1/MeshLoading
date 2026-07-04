#include "HierarchyWindow.h"
#include <imgui.h>
#include "../../Scene/Scene.h"
#include "../../ObjectSystem/PrefabSystem.h"
#include "../../Utils/Debug.h"
#include "../CameraMovement.h"
#include "../EditorWindowRegistry.h"

REGISTER_EDITOR_WINDOW(HierarchyWindow);

void HierarchyWindow::Draw(EditorContext& context)
{
    Scene* scene = context.scene;
    GameObject*& selected = context.selectedGameObject;
    GameObject* editorCamera = context.editorCamera;
     
    ImGui::Begin("Hierarchy");

    if (ImGui::Button("Save"))
        scene->SaveToFile(scene->sceneName + ".json");
    ImGui::SameLine();
    if (ImGui::Button("Add Shape"))
        ImGui::OpenPopup("AddShapePopup");
    if (ImGui::BeginPopup("AddShapePopup")) {
        if (ImGui::MenuItem("Empty"))
            scene->AddGameObject(new GameObject("Empty"));
        if (ImGui::MenuItem("Cube"))
            if (auto* go = PrefabSystem::Instantiate("Assets/Prefabs/Cube.prefab"))
                scene->AddGameObject(go);
        if (ImGui::MenuItem("Sphere"))
            if (auto* go = PrefabSystem::Instantiate("Assets/Prefabs/Sphere.prefab"))
                scene->AddGameObject(go);
        if (ImGui::MenuItem("Capsule"))
            if (auto* go = PrefabSystem::Instantiate("Assets/Prefabs/Capsule.prefab"))
                scene->AddGameObject(go);
        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
            int idx = *(int*)pl->Data;
            auto& a = AssetManager::GetAssets()[idx];
            if (a.type == AssetType::Prefab) {
                GameObject* go = PrefabSystem::Instantiate(a.path);
                scene->AddGameObject(go);
            }
        }
        ImGui::EndDragDropTarget();
    }

    std::function<void(GameObject*)> drawNode = [&](GameObject* obj) {
        ImGui::PushID(obj);
        ImGuiTreeNodeFlags flags = obj->children.empty()
            ? ImGuiTreeNodeFlags_Leaf
            : 0;
        if (obj == selected) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

        if (ImGui::IsItemClicked())
            selected = obj;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            glm::vec3 target = obj->transform.position;
            float     dist = 10.0f;

            editorCamera->transform.position = target + glm::vec3(0, 0, dist);

            glm::vec3 forward = glm::normalize(target - editorCamera->transform.position);
            editorCamera->transform.rotation =
                glm::quatLookAt(forward, glm::vec3(0, 1, 0));

            if (auto* camMove = editorCamera->GetComponent<CameraMovement>()) {
                glm::vec3 euler = glm::degrees(glm::eulerAngles(editorCamera->transform.rotation));
                camMove->yaw = euler.y;
                camMove->pitch = euler.x;

                double mx = InputManager::Instance().GetMouseX();
                double my = InputManager::Instance().GetMouseY();
                camMove->lastX = float(mx);
                camMove->lastY = float(my);
            }
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                if (selected == obj) selected = nullptr;
                scene->RemoveGameObject(obj);
                ImGui::EndPopup();
                if (open)
                    ImGui::TreePop();
                ImGui::PopID();
                return;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_GAMEOBJECT", &obj, sizeof(obj));
            ImGui::Text("Dragging %s", obj->name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (auto pl = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
                GameObject* dropped = *(GameObject**)pl->Data;
                if (dropped->parent) {
                    auto& sib = dropped->parent->children;
                    sib.erase(std::remove(sib.begin(), sib.end(), dropped), sib.end());
                }
                dropped->parent = obj;
                obj->children.push_back(dropped);
            }
            if (auto pl2 = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                int idx = *(int*)pl2->Data;
                auto& a = AssetManager::GetAssets()[idx];
                if (a.type == AssetType::Prefab) {
                    GameObject* go = PrefabSystem::Instantiate(a.path);
                    obj->AddChild(go);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (open) {
            for (auto* child : obj->children)
                drawNode(child);
            ImGui::TreePop();
        }

        ImGui::PopID();
        };

    for (auto* obj : scene->gameObjects) {
        if (!obj->parent) {
            drawNode(obj);
        }
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::InvisibleButton("##hierarchy_space", avail);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto pl = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT"))
        {
            GameObject* dropped = *(GameObject**)pl->Data;
            if (dropped->parent)
            {
                dropped->parent->RemoveChild(dropped);
                selected = dropped;
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

const char* HierarchyWindow::GetWindowName() const
{
    return "Hierarchy";
}
