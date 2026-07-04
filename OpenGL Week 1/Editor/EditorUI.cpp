#include "EditorUI.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>  
#include <imgui.h>
#include <ImGuizmo.h>
#include "../Scene/Scene.h"
#include "../ObjectSystem/GameObject.h"
#include "../Rendering/Camera.h"
#include "../ObjectSystem/AssetManager.h"
#include "InspectorSlotRegistry.h"
#include "../Utils/Debug.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <type_traits>
#include <glm.hpp>
#include "EditorWindowRegistry.h"
#include "UITheme.h"
#include "Windows/ProjectWindow.h"




std::vector<std::unique_ptr<IEditorWindow>> EditorUI::editorWindows;

void EditorUI::Init(GLFWwindow* window, const char* glsl_version)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    UITheme::ApplyDefaultTheme();

    
    const char* comicSansPath = "C:/Windows/Fonts/comic.ttf";
    ImFont* comicSansFont = io.Fonts->AddFontFromFileTTF(comicSansPath, 16.0f);

    if (comicSansFont == nullptr)
    {
        io.Fonts->AddFontDefault();
        DEBUG_ERR("Failed to load Comic Sans font from: " << comicSansPath);
    }

#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    IM_ASSERT(hwnd && "glfwGetWin32Window failed");
#endif

    ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks=*/ true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
void EditorUI::NewFrame() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void EditorUI::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::Shutdown() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void EditorUI::InitializeUI() {
    DEBUG_LOG("Scanning Assets folder...");
    AssetManager::LoadAssets("Assets");

    DEBUG_LOG("Found " << AssetManager::GetAssets().size() << " assets.");
    editorWindows = EditorWindowRegistry::CreateWindows();
    DEBUG_LOG("[EditorUI] Created " << editorWindows.size() << " editor windows.");
}

void EditorUI::ShowDockSpace() {
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

void EditorUI::DrawWindows(EditorContext& context)
{
    for (std::unique_ptr<IEditorWindow>& editorWindow : editorWindows)
    {
        if (editorWindow == nullptr)
        {
            continue;
        }

        editorWindow->Draw(context);
    }
}



void EditorUI::ShowGameObjectNode(GameObject* obj, GameObject*& sel, Scene* scene) {
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
