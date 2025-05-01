#include "UIHelpers.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>  
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
#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <type_traits>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include "CameraMovement.h"
bool UIHelpers::g_SceneViewHovered = false;
bool UIHelpers::g_GameViewHovered = false;
AspectRatio UIHelpers::g_SceneAspect = AspectRatio::R16_9;
AspectRatio UIHelpers::g_GameAspect = AspectRatio::R16_9;
void UIHelpers::Init(GLFWwindow* window, const char* glsl_version)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    IM_ASSERT(hwnd && "glfwGetWin32Window failed");
#endif

    ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks=*/ true);
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
    ImGui::Begin("Scene View");
    {
        static const char* names[] = { "Free","16:9","4:3","1:1" };
        int idx = (int)g_SceneAspect;
        if (ImGui::Combo("Aspect", &idx, names, IM_ARRAYSIZE(names))) {
            g_SceneAspect = (AspectRatio)idx;
        }
    }

    g_SceneViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    editorCamera->Update(deltaTime);

    Camera* cam = editorCamera->GetComponent<Camera>();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 target = avail;
    if (g_SceneAspect != AspectRatio::Free) {
        float ar = 1.0f;
        switch (g_SceneAspect) {
        case AspectRatio::R16_9: ar = 16.0f / 9.0f; break;
        case AspectRatio::R4_3:  ar = 4.0f / 3.0f;  break;
        case AspectRatio::R1_1:  ar = 1.0f;         break;
        default: break;
        }
        if (avail.x / avail.y > ar) {
            target.x = avail.y * ar;
        }
        else {
            target.y = avail.x / ar;
        }
    }

    if ((int)target.x != editorFB->GetWidth() ||
        (int)target.y != editorFB->GetHeight())
    {
        editorFB->Resize((int)target.x, (int)target.y);
        Camera* cam = editorCamera->GetComponent<Camera>();
        cam->InitCamera(target.x, target.y);
    }

    editorFB->Bind();
    glViewport(0, 0, (int)target.x, (int)target.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->Render(editorFB, cam);
    scene->DrawGizmos(cam);
    editorFB->Unbind();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - target.x) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (avail.y - target.y) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)editorFB->GetTextureID(), target);

    ImVec2 img_min = ImGui::GetItemRectMin();
    ImVec2 img_max = ImGui::GetItemRectMax();
    ImVec2 img_sz{ img_max.x - img_min.x, img_max.y - img_min.y };

    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(selected != nullptr);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(img_min.x, img_min.y, img_sz.x, img_sz.y);

    glm::mat4 view = cam->GetViewMatrix();
    glm::mat4 proj = cam->GetProjectionMatrix();
    proj[1][1] *= -1.0f; 

    if (selected) {
        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        glm::mat4 model = selected->GetWorldMatrix();

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
    Scene* scene,
    EditorState& state)
{

    ImGui::Begin("Game View");

    g_GameViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    if (ImGui::Button("Stop"))  state = EditorState::Stop;
    ImGui::SameLine();
    if (ImGui::Button("Play"))  state = EditorState::Play;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) state = EditorState::Pause;
    ImGui::SameLine();
    {
        static const char* names[] = { "Free","16:9","4:3","1:1" };
        int idx = (int)g_GameAspect;
        if (ImGui::Combo("Aspect", &idx, names, IM_ARRAYSIZE(names))) {
            g_GameAspect = (AspectRatio)idx;
        }
    }
    if (state == EditorState::Play)
        ImGui::TextColored({ 0,1,0,1 }, "PLAY MODE");

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 target = avail;
    if (g_GameAspect != AspectRatio::Free) {
        float ar = (g_GameAspect == AspectRatio::R16_9 ? 16.f / 9.f :
            g_GameAspect == AspectRatio::R4_3 ? 4.f / 3.f : 1.f);
        if (avail.x / avail.y > ar) target.x = avail.y * ar;
        else                      target.y = avail.x / ar;
    }

    if ((int)target.x != gameFB->GetWidth() ||
        (int)target.y != gameFB->GetHeight())
    {
        gameFB->Resize((int)target.x, (int)target.y);
        if (auto cam = scene->camera)
            cam->InitCamera(target.x, target.y);
    }

    gameFB->Bind();
    glViewport(0, 0, (int)target.x, (int)target.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->Render(gameFB, nullptr);
    gameFB->Unbind();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - target.x) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (avail.y - target.y) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)gameFB->GetTextureID(), target);

    ImGui::End();
}

void UIHelpers::ShowGameObjectNode(GameObject* obj, GameObject*& sel, Scene* scene) {
    ImGui::PushID(obj);

    ImGuiTreeNodeFlags flags = obj->children.empty()
        ? ImGuiTreeNodeFlags_Leaf
        : 0;
    if (obj == sel)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

    if (ImGui::IsItemClicked())
        sel = obj;

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            if (sel == obj) sel = nullptr;
            scene->RemoveGameObject(obj);
            ImGui::EndPopup();
            if (open) ImGui::TreePop();
            ImGui::PopID();
            return; 
        }
        ImGui::EndPopup();
    }


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
 
void UIHelpers::DrawProjectWindow() {
    ImGui::Begin("Project");

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

    std::function<void(const fs::path&)> drawFolder =
        [&](const fs::path& dir)
        {
            for (auto& entry : fs::directory_iterator(dir)) {
                const auto name = entry.path().filename().string();

                if (entry.is_directory()) {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                    ImGui::PushID(entry.path().string().c_str());
                    bool open = ImGui::TreeNodeEx(name.c_str(), flags, "%s/", name.c_str());

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
                    ImGui::PushID(entry.path().string().c_str());
                    ImGui::Selectable(name.c_str(), false);

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
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::Separator();
    ImGui::BeginChild("##DebugScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    const auto& entries = Debug::GetEntries();
    for (const auto& e : entries) {
        ImVec4 col;
        switch (e.level) {
        case DebugLevel::Log:       col = ImVec4(1, 1, 1, 1);    break; // white
        case DebugLevel::Warning:   col = ImVec4(1, 1, 0, 1);    break; // yellow
        case DebugLevel::Error:     col = ImVec4(1, 0, 0, 1);    break; // red
        case DebugLevel::Exception: col = ImVec4(1, 0.5f, 0, 1); break; // orange
        }
        ImGui::PushStyleColor(ImGuiCol_Text, col);

        const char* lvlName = "UNK";
        switch (e.level) {
        case DebugLevel::Log:       lvlName = "LOG";  break;
        case DebugLevel::Warning:   lvlName = "WARN"; break;
        case DebugLevel::Error:     lvlName = "ERR";  break;
        case DebugLevel::Exception: lvlName = "EXPT"; break;
        }

        ImGui::Text("[%s] [%s] %s:%d  %s",
            e.timestamp.c_str(),
            lvlName,
            e.file.c_str(),
            e.line,
            e.message.c_str()
        );

        ImGui::PopStyleColor();
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
