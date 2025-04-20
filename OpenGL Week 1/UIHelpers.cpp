#include "UIHelpers.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <iostream>
#include "FrameBuffer.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "AssetManager.h"
#include "PrefabSystem.h"
#include "ScriptComponent.h"

extern GameObject* editorCamera;

namespace UIHelpers {

    void InitializeUI() {
        std::cout << "[UIHelpers] Scanning Assets folder..." << std::endl;
        AssetManager::LoadAssets("Assets");
        std::cout << "[UIHelpers] Found "
            << AssetManager::GetAssets().size()
            << " assets."
            << std::endl;

    }

    void ShowDockSpace() {
        ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);
        ImGui::SetNextWindowViewport(vp->ID);
        flags |= ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpace", nullptr, flags);
        ImGui::DockSpace(ImGui::GetID("MyDockSpace"));
        ImGui::End();
    }
    void DrawSceneViewWindow(FrameBuffer* editorFB,
        GameObject*       /*editorCamera*/,
        Scene* scene,
        GameObject* selected,
        float             deltaTime)
    {
        // 1) Render 3D scene into our FBO
        editorCamera->Update(deltaTime);
        editorFB->Bind();
        scene->Render(editorFB, editorCamera->GetComponent<Camera>());
        editorFB->Unbind();

        // 2) Begin ImGui window
        ImGui::Begin("Scene View");

        // 2a) Draw the texture
        ImVec2 sz((float)editorFB->GetWidth(), (float)editorFB->GetHeight());
        ImGui::Image((ImTextureID)(intptr_t)editorFB->GetTextureID(), sz);

        // 2b) Grab its on‑screen rectangle
        ImVec2 img_min = ImGui::GetItemRectMin();
        ImVec2 img_max = ImGui::GetItemRectMax();
        ImVec2 img_sz{ img_max.x - img_min.x, img_max.y - img_min.y };

        // 3) Setup ImGuizmo
        ImGuizmo::BeginFrame();
        ImGuizmo::Enable(selected != nullptr);
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(img_min.x, img_min.y, img_sz.x, img_sz.y);

        // 4) Fetch camera matrices
        Camera* cam = editorCamera->GetComponent<Camera>();
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 proj = cam->GetProjectionMatrix();
        proj[1][1] *= -1.0f; // flip Y for ImGui

        // 5) If an object is selected, draw gizmo
        if (selected) {
            ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
            glm::mat4 model = selected->GetWorldMatrix();

            // confine draw
            ImGui::PushClipRect(img_min, img_max, true);
            bool depth = glIsEnabled(GL_DEPTH_TEST);
            if (depth) glDisable(GL_DEPTH_TEST);

            ImGuizmo::Manipulate(glm::value_ptr(view),
                glm::value_ptr(proj),
                op,
                ImGuizmo::LOCAL,
                glm::value_ptr(model));

            if (depth) glEnable(GL_DEPTH_TEST);
            ImGui::PopClipRect();

            if (ImGuizmo::IsUsing()) {
                glm::vec3 t, r, s;
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model),
                    glm::value_ptr(t),
                    glm::value_ptr(r),
                    glm::value_ptr(s));
                selected->transform.position = t;
                selected->transform.rotation = glm::quat(glm::radians(r));
                selected->transform.scale = s;
            }
        }

        // 6) Allow dropping a .prefab anywhere into the scene view
        if (ImGui::BeginDragDropTarget()) {
            if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                int idx = *(int*)pl->Data;
                auto& a = AssetManager::GetAssets()[idx];
                if (a.type == AssetType::Prefab) {
                    GameObject* go = PrefabSystem::Instantiate(a.path);
                    scene->AddGameObject(go);
                    std::cout << "[UIHelpers] Instantiated prefab '"
                        << a.name << "' into Scene View\n";
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
    }
    void DrawGameViewWindow(FrameBuffer* gameFB,
        GameObject*,
        Scene* scene,
        EditorState& state,
        float         deltaTime)
    {
        editorCamera->Update(deltaTime);
        gameFB->Bind();
        scene->Render(gameFB, editorCamera->GetComponent<Camera>());
        gameFB->Unbind();

        ImGui::Begin("Game View");
        if (ImGui::Button("Play"))  state = EditorState::Play;
        ImGui::SameLine();
        if (ImGui::Button("Pause")) state = EditorState::Pause;
        ImGui::SameLine();
        if (ImGui::Button("Stop"))  state = EditorState::Stop;

        ImVec2 sz((float)gameFB->GetWidth(), (float)gameFB->GetHeight());
        ImGui::Image((ImTextureID)(intptr_t)gameFB->GetTextureID(), sz);
        ImGui::Text("This is where the scene is rendered.");
        ImGui::End();
    }

    void ShowGameObjectNode(GameObject* obj, GameObject*& sel, Scene* scene)
    {
        // Push a unique ID scope (we use the pointer value here)
        ImGui::PushID(obj);

        ImGuiTreeNodeFlags flags = obj->children.empty()
            ? ImGuiTreeNodeFlags_Leaf
            : 0;
        if (obj == sel)
            flags |= ImGuiTreeNodeFlags_Selected;

        // The visible label is just obj->name, the ID is hidden behind PushID
        bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

        // single‐click selects
        if (ImGui::IsItemClicked())
            sel = obj;

        // double‐click frames camera
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            glm::vec3 target = obj->transform.position;
            float distance = 10.f;
            editorCamera->transform.position = target + glm::vec3(0, 0, distance);
            glm::vec3 forward = glm::normalize(target - editorCamera->transform.position);
            editorCamera->transform.rotation =
                glm::quatLookAt(forward, -glm::vec3(0, 1, 0));
        }

        // drag source for reparenting existing GameObjects
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_GAMEOBJECT", &obj, sizeof(obj));
            ImGui::Text("Dragging %s", obj->name.c_str());
            ImGui::EndDragDropSource();
        }

        // drag target: accept either reparent or prefab instantiation
        if (ImGui::BeginDragDropTarget()) {
            // A) reparent
            if (auto pl = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
                GameObject* dropped = *(GameObject**)pl->Data;
                if (dropped->parent) {
                    auto& sib = dropped->parent->children;
                    sib.erase(std::remove(sib.begin(), sib.end(), dropped),
                        sib.end());
                }
                dropped->parent = obj;
                obj->children.push_back(dropped);
            }
            // B) instantiate prefab under this node
            if (auto pl2 = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                int idx = *(int*)pl2->Data;
                auto& a = AssetManager::GetAssets()[idx];
                if (a.type == AssetType::Prefab) {
                    GameObject* go = PrefabSystem::Instantiate(a.path);
                    obj->AddChild(go);
                    std::cout << "[UIHelpers] Instantiated prefab '"
                        << a.name << "' under '" << obj->name << "'\n";
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Recurse into children
        if (open) {
            for (auto* c : obj->children)
                ShowGameObjectNode(c, sel, scene);
            ImGui::TreePop();
        }

        // Pop the ID scope
        ImGui::PopID();
    }

    void DrawHierarchyWindow(Scene* scene, GameObject*& selected) {
        ImGui::Begin("Hierarchy");

        if (ImGui::Button("Save"))
            scene->SaveToFile(scene->sceneName + ".json");
        ImGui::SameLine();
        if (ImGui::Button("Add Empty"))
            scene->AddGameObject(new GameObject("Empty"));

        // allow dropping prefab onto blank area
        if (ImGui::BeginDragDropTarget()) {
            if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                int idx = *(int*)pl->Data;
                auto& a = AssetManager::GetAssets()[idx];
                if (a.type == AssetType::Prefab) {
                    GameObject* go = PrefabSystem::Instantiate(a.path);
                    scene->AddGameObject(go);
                    std::cout << "[UIHelpers] Instantiated prefab '"
                        << a.name << "' as root\n";
                }
            }
            ImGui::EndDragDropTarget();
        }

        // draw the tree
        for (auto* obj : scene->gameObjects) {
            if (!obj->parent)
                ShowGameObjectNode(obj, selected, scene);
        }
        ImGui::End();
    }


    void DrawInspectorWindow(GameObject*& selected) {
        ImGui::Begin("Inspector");
        if (selected) {
            selected->OnInspectorGUI();

            ImGui::Separator();
            ImGui::Text("Drag asset here to add:");
            if (ImGui::BeginDragDropTarget()) {
                if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                    int idx = *(int*)pl->Data;
                    auto& a = AssetManager::GetAssets()[idx];
                    std::cout << "[UIHelpers] Inspector got asset: " << a.name << "\n";
                    if (a.type == AssetType::Script) {
                        selected->AddComponent<ScriptComponent>(a.path);
                    }
                    else {
                        selected->AddChild(PrefabSystem::Instantiate(a.path));
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

    void DrawProjectWindow() {
        ImGui::Begin("Project");

        ImGui::TextUnformatted("• Drop a GameObject here to save a Prefab");
        ImGui::TextUnformatted("• Drag a .prefab below into Hierarchy/Scene View");

        // 1) drop‑target to save a live GameObject → prefab
        if (ImGui::BeginDragDropTarget()) {
            if (auto pl = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
                GameObject* go = *(GameObject**)pl->Data;
                std::string safe = go->name;
                for (char& c : safe) if (c == ' ') c = '_';
                std::string path = "Assets/Prefabs/" + safe + ".prefab";
                std::cout << "[UIHelpers] Saving Prefab: " << path << "\n";
                if (PrefabSystem::SavePrefab(go, path))
                    AssetManager::LoadAssets("Assets");
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Separator();

        // 2) list all assets; make only Prefabs drag sources
        const auto& assets = AssetManager::GetAssets();
        for (int i = 0; i < (int)assets.size(); ++i) {
            ImGui::PushID(i);
            ImGui::TextUnformatted(assets[i].name.c_str());
            if (assets[i].type == AssetType::Prefab) {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("ASSET_PAYLOAD", &i, sizeof(i));
                    ImGui::Text("Instantiate %s", assets[i].name.c_str());
                    ImGui::EndDragDropSource();
                }
            }
            ImGui::PopID();
        }

        ImGui::End();
    }



    void UIHelpers::DrawDebugWindow(bool* p_open) {
        ImGui::Begin("Console", p_open, ImGuiWindowFlags_HorizontalScrollbar);

        if (ImGui::Button("Clear")) {
            Debug::ClearEntries();
        }
        ImGui::SameLine();
        static bool autoScroll = true;
        ImGui::Checkbox("Auto‑scroll", &autoScroll);
        ImGui::Separator();

        ImGui::BeginChild("##DebugScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& entries = Debug::GetEntries();
        for (const auto& e : entries) {
            ImVec4 col;
            switch (e.level) {
            case DebugLevel::Info:      col = ImVec4(1, 1, 1, 1);     break;
            case DebugLevel::Warning:   col = ImVec4(1, 1, 0, 1);     break;
            case DebugLevel::Error:     col = ImVec4(1, 0, 0, 1);     break;
            case DebugLevel::Exception: col = ImVec4(1, 0.5f, 0, 1);  break;
            case DebugLevel::Assertion: col = ImVec4(1, 0, 0.5f, 1);  break;
            }
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::Text("[%s] %s", e.timestamp.c_str(), e.message.c_str());
            ImGui::PopStyleColor();
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }

}