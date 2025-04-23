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
#include "InspectorSlotRegistry.h"
#include "ComponentFactory.h"
extern GameObject* editorCamera;
bool UIHelpers::g_SceneViewHovered = false;
bool UIHelpers::g_GameViewHovered = false;
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
        g_SceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
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
        g_GameViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
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
    void UIHelpers::ShowGameObjectNode(GameObject* obj, GameObject*& sel, Scene* scene) {
        ImGui::PushID(obj);

        ImGuiTreeNodeFlags flags = obj->children.empty()
            ? ImGuiTreeNodeFlags_Leaf
            : 0;
        if (obj == sel)
            flags |= ImGuiTreeNodeFlags_Selected;

        // The visible label is just obj->name
        bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

        // single‐click selects
        if (ImGui::IsItemClicked())
            sel = obj;

        // right‐click context menu → Delete
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                // if it was selected, clear selection
                if (sel == obj) sel = nullptr;
                // remove & delete from scene
                scene->RemoveGameObject(obj);
                ImGui::EndPopup();
                if (open) ImGui::TreePop();
                ImGui::PopID();
                return; // don’t draw children of something we just deleted
            }
            ImGui::EndPopup();
        }

        // double‐click to frame camera, drag/drop for reparenting or prefab as before…
        // (your existing code for ImGui::IsMouseDoubleClicked,
        //  BeginDragDropSource, BeginDragDropTarget, etc.)

        if (open) {
            for (auto* child : obj->children)
                ShowGameObjectNode(child, sel, scene);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
    void UIHelpers::DrawHierarchyWindow(Scene* scene, GameObject*& selected) {
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

        // blank‑space drop target for root‑level prefab instantiation
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

        // recursive node‑drawer captures scene & selected by reference
        std::function<void(GameObject*)> drawNode = [&](GameObject* obj) {
            ImGui::PushID(obj);
            ImGuiTreeNodeFlags flags = obj->children.empty()
                ? ImGuiTreeNodeFlags_Leaf
                : 0;
            if (obj == selected) flags |= ImGuiTreeNodeFlags_Selected;

            bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

            // single‐click to select
            if (ImGui::IsItemClicked())
                selected = obj;

            // double‐click to frame camera
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                glm::vec3 target = obj->transform.position;
                float dist = 10.0f;
                editorCamera->transform.position = target + glm::vec3(0, 0, dist);
                glm::vec3 forward = glm::normalize(target - editorCamera->transform.position);
                editorCamera->transform.rotation = glm::quatLookAt(forward, -glm::vec3(0, 1, 0));
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
            // drag source for reparenting
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("DND_GAMEOBJECT", &obj, sizeof(obj));
                ImGui::Text("Dragging %s", obj->name.c_str());
                ImGui::EndDragDropSource();
            }

            // drag target: reparent or prefab‑under‑node
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

        // draw all root objects
        for (auto* obj : scene->gameObjects)
            if (!obj->parent)
                drawNode(obj);

        ImGui::End();
    }

    void UIHelpers::DrawInspectorWindow(GameObject*& selected) {
        ImGui::Begin("Inspector");
        if (selected) {
            // —— Rename field ——————————————————————————————
            char buf[128];
            strncpy_s(buf, selected->name.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText("##Rename", buf, sizeof(buf))) {
                selected->name = buf;
            }
            ImGui::SameLine();
            ImGui::Text("Name");
            ImGui::Separator();
            // ————————————————————————————————————————————————————

            // —— Transform ——————————————————————————————
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat3("Position", glm::value_ptr(selected->transform.position), 0.1f);
                glm::vec3 euler = glm::degrees(glm::eulerAngles(selected->transform.rotation));
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.5f)) {
                    selected->transform.rotation = glm::quat(glm::radians(euler));
                }
                ImGui::DragFloat3("Scale", glm::value_ptr(selected->transform.scale), 0.1f);
            }
            ImGui::Separator();
            // ————————————————————————————————————————————————————

            // —— Components + asset‑slots ——————————————————————
            for (auto& compPtr : selected->components) {
                Component* comp = compPtr.get();
                std::string label = typeid(*comp).name();
                if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    // 1) any asset‑slots you registered
                    for (auto& slot : InspectorSlotRegistry::GetSlotsFor(comp)) {
                        ImGui::Text("%s", slot.label.c_str());
                        ImGui::SameLine();
                        ImGui::PushID(slot.label.c_str());
                        std::string cur = slot.getter(comp);
                        if (ImGui::Button(cur.empty() ? "<none>" : cur.c_str(), ImVec2(-1, 0))) {}
                        if (ImGui::BeginDragDropTarget()) {
                            if (auto pd = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                                int idx = *(int*)pd->Data;
                                auto& a = AssetManager::GetAssets()[idx];
                                if (std::find(slot.acceptedTypes.begin(),
                                    slot.acceptedTypes.end(),
                                    a.type)
                                    != slot.acceptedTypes.end())
                                {
                                    slot.setter(comp, a);
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                        ImGui::PopID();
                    }
                    ImGui::Separator();
                    // 2) the component’s own fields
                    comp->OnInspectorGUI();
                    ImGui::Separator();
                }
            }

         // —— Add Component popup ——————————————————————
            if (ImGui::Button("Add Component…"))
                ImGui::OpenPopup("AddComponentPopup");

            if (ImGui::BeginPopup("AddComponentPopup")) {
                std::cout << "[UI]  AddComponentPopup opened, registry size="
                    << ComponentFactory::Instance().GetRegistry().size() << "\n";

                for (auto& [typeName, entry] : ComponentFactory::Instance().GetRegistry()) {
                    if (ImGui::MenuItem(typeName.c_str())) {
                        std::cout << "[UI]  MenuItem clicked: \"" << typeName << "\"\n";
                        Component* newComp = ComponentFactory::Instance().Create(
                            typeName,
                            nlohmann::json::object(),
                            selected
                        );
                        std::cout <<
                            "[UI]  After Create: newComp=" << newComp << "\n";

                        // only close the popup after we’ve logged
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::Separator();

            // —— Drag‑drop to add scripts / prefabs ——————————————————
            ImGui::Text("Drag asset here to add:");
            if (ImGui::BeginDragDropTarget()) {
                if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
                    int idx = *(int*)pl->Data;
                    auto& a = AssetManager::GetAssets()[idx];
                    if (a.type == AssetType::Script) {
                        selected->AddComponent<ScriptComponent>(a.path);
                    }
                    else if (a.type == AssetType::Prefab) {
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

    
    void UIHelpers::DrawProjectWindow() {
        ImGui::Begin("Project");

        // --- Drag‐target for saving a live GameObject as a prefab ---
        ImGui::TextUnformatted("Drop a GameObject here to save a Prefab");
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

        // --- List all assets ---
        const auto& assets = AssetManager::GetAssets();
        for (int i = 0; i < (int)assets.size(); ++i) {
            const Asset& a = assets[i];
            ImGui::PushID(i);

            // Show asset name
            ImGui::Selectable(a.name.c_str(), false);

            // 1) Make it a drag‑source for ANY asset
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("ASSET_PAYLOAD", &i, sizeof(i));
                ImGui::Text("%s (%s)", a.name.c_str(),
                    a.type == AssetType::Texture ? "Texture" :
                    a.type == AssetType::Model ? "Model" :
                    a.type == AssetType::Script ? "Script" :
                    a.type == AssetType::Prefab ? "Prefab" : "Unknown");
                ImGui::EndDragDropSource();
            }

            // 2) Context‐menu on *any* asset
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Rename")) {
                    static char buf[128];
                    strncpy_s(buf, a.name.c_str(), sizeof(buf));
                    ImGui::OpenPopup("Rename Asset");
                    ImGui::SetNextWindowPos(ImGui::GetMousePos());
                    // store renameIndex / buf somewhere...
                }
                if (ImGui::MenuItem("Delete")) {
                    fs::remove(a.path);
                    AssetManager::LoadAssets("Assets");
                    ImGui::EndPopup();
                    ImGui::PopID();
                    // immediately break out of this loop since assets[] has changed
                    break;
                }
                if (ImGui::MenuItem("Reveal in Explorer")) {
                  //  Utils::OpenInExplorer(a.path);
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        // 3) (Optional) Rename‑modal
        if (ImGui::BeginPopupModal("Rename Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char renameBuf[128] = "";
            ImGui::InputText("New Name", renameBuf, sizeof(renameBuf));
            if (ImGui::Button("OK", ImVec2(80, 0))) {
                // compute new path, rename file, reload assets...
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(80, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
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