#include "UIHelpers.h"
#include "AssetManager.h"   
#include "PrefabSystem.h"   
#include <imgui.h>
#include "ScriptComponent.h"

namespace UIHelpers {

    void InitializeUI() {
        AssetManager::LoadAssets("Assets");
    }

    void ShowDockSpace() {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);
        ImGui::SetNextWindowViewport(vp->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::DockSpace(ImGui::GetID("MyDockSpace"));
        ImGui::End();
    }

    void DrawSceneViewWindow(FrameBuffer* editorFB, Camera* editorCam) {
        ImGui::Begin("Scene View");
        editorCam->Update(0.016f);
        ImVec2 sz(editorFB->GetWidth(), editorFB->GetHeight());
        editorFB->Bind(); // assume Render done externally
        ImGui::Image((ImTextureID)(intptr_t)editorFB->GetTextureID(), sz);
        ImGui::End();
    }

    void DrawGameViewWindow(FrameBuffer* gameFB, Camera* gameCam, int& editorState) {
        ImGui::Begin("Game View");
        if (ImGui::Button("Play")) editorState = 0;
        ImGui::SameLine();
        if (ImGui::Button("Pause")) editorState = 1;
        ImGui::SameLine();
        if (ImGui::Button("Stop")) editorState = 2;
        ImVec2 sz(gameFB->GetWidth(), gameFB->GetHeight());
        gameFB->Bind();
        ImGui::Image((ImTextureID)(intptr_t)gameFB->GetTextureID(), sz);
        ImGui::End();
    }

    void DrawInspectorWindow(GameObject*& selected) {
        ImGui::Begin("Inspector");
        if (selected) {
            selected->OnInspectorGUI();
            // Accept asset drag/drop
            ImGui::Separator();
            ImGui::Text("Drag asset here to add:");
            if (ImGui::BeginDragDropTarget()) {
                if (auto payload = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                    int idx = *(const int*)payload->Data;
                    auto& a = AssetManager::GetAssets()[idx];
                    if (a.type == AssetType::Script) {
                        selected->AddComponent<ScriptComponent>(a.path);
                    }
                    else if (a.type == AssetType::Prefab) {
                        GameObject* inst = PrefabSystem::Instantiate(a.path);
                        selected->AddChild(inst);
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
        else {
            ImGui::Text("Nothing selected.");
        }
        ImGui::End();
    }

    void ShowGameObjectNode(GameObject* obj, GameObject*& sel) {
        ImGuiTreeNodeFlags flags = obj->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
        if (obj == sel) flags |= ImGuiTreeNodeFlags_Selected;
        bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);
        if (ImGui::IsItemClicked()) sel = obj;
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_GAMEOBJECT", &obj, sizeof(obj));
            ImGui::Text("Dragging %s", obj->name.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()) {
            if (auto payload = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
                GameObject* dropped = *(GameObject**)payload->Data;
                if (dropped->parent) {
                    auto& sib = dropped->parent->children;
                    sib.erase(std::remove(sib.begin(), sib.end(), dropped), sib.end());
                }
                dropped->parent = obj;
                obj->children.push_back(dropped);
            }
            ImGui::EndDragDropTarget();
        }
        if (open) {
            for (auto* c : obj->children) ShowGameObjectNode(c, sel);
            ImGui::TreePop();
        }
    }

    void DrawHierarchyWindow(Scene* scene, GameObject*& selected) {
        ImGui::Begin("Hierarchy");
        if (ImGui::Button("Save")) scene->SaveToFile(scene->sceneName + ".json");
        if (ImGui::Button("Add Empty")) scene->AddGameObject(new GameObject("Empty"));
        // accept prefab drop to root
        if (ImGui::BeginDragDropTarget()) {
            if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                int idx = *(int*)pl->Data;
                auto& a = AssetManager::GetAssets()[idx];
                if (a.type == AssetType::Prefab) {
                    scene->AddGameObject(PrefabSystem::Instantiate(a.path));
                }
            }
            ImGui::EndDragDropTarget();
        }
        for (auto* obj : scene->gameObjects) if (!obj->parent)
            ShowGameObjectNode(obj, selected);
        ImGui::End();
    }

    void DrawProjectWindow() {
        ImGui::Begin("Project");
        const auto& assets = AssetManager::GetAssets();
        for (int i = 0; i < assets.size(); ++i) {
            ImGui::PushID(i);
            ImGui::Text("%s", assets[i].name.c_str());
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("ASSET_PAYLOAD", &i, sizeof(i));
                ImGui::Text("Dragging %s", assets[i].name.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    void DrawConsoleWindow() {
        ImGui::Begin("Console");
        ImGui::Text("Console panel");
        ImGui::End();
    }

} // namespace UIHelpers
