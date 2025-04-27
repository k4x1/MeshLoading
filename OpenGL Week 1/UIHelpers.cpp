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
#include "InspectorSlotRegistry.h"
#include "ComponentFactory.h"
#include "Debug.h"
bool UIHelpers::g_SceneViewHovered = false;
bool UIHelpers::g_GameViewHovered = false;


static bool s_glfw_inited = false;
void UIHelpers::Init(GLFWwindow*& window, const char* glsl_version) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks=*/ !s_glfw_inited);
    s_glfw_inited = true;
    ImGui_ImplOpenGL3_Init(glsl_version);

}

void UIHelpers::NewFrame() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void UIHelpers::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIHelpers::Shutdown() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}
ImGuiContext* UIHelpers::GetImGuiContext()
{
    return ImGui::GetCurrentContext();
}
void UIHelpers::SetImGuiContext(ImGuiContext* context)
{
    ImGui::SetCurrentContext(context);
    ImGuizmo::SetImGuiContext(context);
}
void UIHelpers::InitializeUI() {
    std::cout << "[UIHelpers] Scanning Assets folder..." << std::endl;
    AssetManager::LoadAssets("Assets");
    std::cout << "[UIHelpers] Found "
        << AssetManager::GetAssets().size()
        << " assets."
        << std::endl;

}

void UIHelpers::ShowDockSpace() {
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
void UIHelpers::DrawSceneViewWindow(FrameBuffer* editorFB,
    GameObject* editorCamera,
    Scene* scene,
    GameObject* selected,
    float       deltaTime)
{
    // 1) update & render your editor camera into the FBO
    editorCamera->Update(deltaTime);
    editorFB->Bind();
    scene->Render(editorFB, editorCamera->GetComponent<Camera>());
    editorFB->Unbind();

    // 2) ImGui window
    ImGui::Begin("Scene View");
    g_SceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    // 2a) draw the texture
    ImVec2 sz{ (float)editorFB->GetWidth(), (float)editorFB->GetHeight() };
    ImGui::Image((ImTextureID)(intptr_t)editorFB->GetTextureID(), sz);

    // 2b) gizmo setup
    ImVec2 img_min = ImGui::GetItemRectMin();
    ImVec2 img_max = ImGui::GetItemRectMax();
    ImVec2 img_sz{ img_max.x - img_min.x, img_max.y - img_min.y };

    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(selected != nullptr);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(img_min.x, img_min.y, img_sz.x, img_sz.y);

    Camera* cam = editorCamera->GetComponent<Camera>();
    glm::mat4 view = cam->GetViewMatrix();
    glm::mat4 proj = cam->GetProjectionMatrix();
    proj[1][1] *= -1.0f;   // flip Y for ImGuizmo

    if (selected) {
        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        glm::mat4 model = selected->GetWorldMatrix();

        // confine draw to the scene-view rectangle
        ImGui::PushClipRect(img_min, img_max, true);
        bool wasDepth = glIsEnabled(GL_DEPTH_TEST);
        if (wasDepth) glDisable(GL_DEPTH_TEST);

        ImGuizmo::Manipulate(glm::value_ptr(view),
            glm::value_ptr(proj),
            op,
            ImGuizmo::LOCAL,
            glm::value_ptr(model));

        if (wasDepth) glEnable(GL_DEPTH_TEST);
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

    // 3) drag-and-drop prefabs into scene
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

    ImGui::End();
}

void UIHelpers::DrawGameViewWindow(FrameBuffer* gameFB,
    GameObject* gameCamera,
    Scene* scene,
    EditorState& state,
    float       deltaTime)
{
    // 1) advance the in-game camera and render the shared scene
    gameCamera->Update(deltaTime);
    gameFB->Bind();
    scene->Render(gameFB, gameCamera->GetComponent<Camera>());
    gameFB->Unbind();

    // 2) UI
    ImGui::Begin("Game View");
    g_GameViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    if (ImGui::Button("Stop"))  state = EditorState::Stop;
    ImGui::SameLine();
    if (ImGui::Button("Play"))  state = EditorState::Play;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) state = EditorState::Pause;

    if (state == EditorState::Play)
        ImGui::TextColored({ 0,1,0,1 }, "● PLAY MODE");

    ImVec2 sz{ (float)gameFB->GetWidth(), (float)gameFB->GetHeight() };
    ImGui::Image((ImTextureID)(intptr_t)gameFB->GetTextureID(), sz);
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

void UIHelpers::DrawHierarchyWindow(Scene* scene, GameObject*& selected, GameObject* editorCamera) {
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

    // —— Components + asset‐slots + right‐click to remove —————————————————
    for (int i = 0; i < (int)selected->components.size(); ++i) {
        Component* comp = selected->components[i].get();
        ImGui::PushID(comp);
        bool open = ImGui::CollapsingHeader(typeid(*comp).name(), ImGuiTreeNodeFlags_DefaultOpen);

        // Right‐click to remove
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
            // any registered asset‐slots (e.g. for textures, models…)
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
            // component’s own fields
            comp->OnInspectorGUI();
            ImGui::Separator();
        }
        ImGui::PopID();
    }

    // —— “Add Component…” popup ——————————————————————
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

    
void UIHelpers::DrawProjectWindow() {
    ImGui::Begin("Project");

    // —— New Folder button + modal —————————————
    if (ImGui::Button("New Folder")) {
        ImGui::OpenPopup("Create New Folder");
    }
    ImGui::SameLine();

    if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char folderName[128] = "";
        ImGui::InputText("Folder Name", folderName, sizeof(folderName));
        if (ImGui::Button("OK", ImVec2(80, 0))) {
            fs::path newDir = fs::path("Assets") / folderName;
            std::error_code ec;
            if (!fs::create_directory(newDir, ec)) {
                std::cerr << "[UIHelpers] Failed to create folder: "
                    << newDir.string() << " (" << ec.message() << ")\n";
            }
            else {
                AssetManager::LoadAssets("Assets");
            }
            folderName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            folderName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

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

    // —— Recursive folder+file tree ——————————————
    std::function<void(const fs::path&)> drawFolder =
        [&](const fs::path& dir)
        {
            for (auto& entry : fs::directory_iterator(dir)) {
                const auto name = entry.path().filename().string();

                if (entry.is_directory()) {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                    ImGui::PushID(entry.path().string().c_str());
                    bool open = ImGui::TreeNodeEx(name.c_str(), flags, "%s/", name.c_str());

                    // context menu on folder
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Rename Folder")) {
                            // TODO: implement rename-folder logic
                        }
                        if (ImGui::MenuItem("Delete Folder")) {
                            std::error_code ec;
                            fs::remove_all(entry.path(), ec);
                            if (ec)
                                std::cerr << "[UIHelpers] Failed to delete folder: "
                                << entry.path().string() << "\n";
                            AssetManager::LoadAssets("Assets");
                        }
                        ImGui::EndPopup();
                    }

                    if (open) {
                        drawFolder(entry.path());
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                else {
                    // it's a file
                    ImGui::PushID(entry.path().string().c_str());
                    ImGui::Selectable(name.c_str(), false);

                    // drag-source for asset
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                        const auto& assets = AssetManager::GetAssets();
                        for (int i = 0; i < (int)assets.size(); ++i) {
                            if (assets[i].path == entry.path().string()) {
                                ImGui::SetDragDropPayload("ASSET_PAYLOAD", &i, sizeof(i));
                                ImGui::Text("%s", name.c_str());
                                break;
                            }
                        }
                        ImGui::EndDragDropSource();
                    }

                    // context menu on file
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Rename File")) {
                            // TODO: implement rename-file logic
                        }
                        if (ImGui::MenuItem("Delete File")) {
                            std::error_code ec;
                            fs::remove(entry.path(), ec);
                            if (ec)
                                std::cerr << "[UIHelpers] Failed to delete file: "
                                << entry.path().string() << "\n";
                            AssetManager::LoadAssets("Assets");
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
            }
        };

    // draw starting at the root Assets folder:
    drawFolder("Assets");

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

