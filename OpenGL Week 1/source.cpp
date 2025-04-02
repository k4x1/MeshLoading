#define GLM_ENABLE_EXPERIMENTAL
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>
#include "SampleScene.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "FrameBuffer.h"
#include "GameObject.h"
#include "CameraMovement.h"
#include "InputManager.h"  

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

// Global variables
std::unique_ptr<Scene> CurrentScene;
GLFWwindow* Window = nullptr;
InputManager* inputs = nullptr;
FrameBuffer* editorFrameBuffer;
GameObject* editorCamera;
FrameBuffer* frameBuffer;
GameObject* selectedGameObject = nullptr;
int emptyObjCount = 0;

enum class SceneType {
    Game,
};

void switchScene(SceneType sceneType) {
    switch (sceneType) {
    case SceneType::Game:
        CurrentScene = std::make_unique<SampleScene>();
        std::cout << "Shadow Scene" << std::endl;
        break;
    default:
        break;
    }
    CurrentScene->InitialSetup(Window);
}

enum class EditorState {
    Play,
    Pause,
    Stop
};

GameObject* renamingGameObject = nullptr;
char renameBuffer[256] = { 0 };
bool shouldOpenRenamePopup = false;

void ShowGameObjectNode(GameObject* gameObject, GameObject*& selected) {
    ImGuiTreeNodeFlags flags = gameObject->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    if (gameObject == selected)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool open = ImGui::TreeNodeEx(gameObject->name.c_str(), flags);
    if (ImGui::IsItemClicked())
        selected = gameObject;

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("DND_GAMEOBJECT", &gameObject, sizeof(GameObject*));
        ImGui::Text("Dragging %s", gameObject->name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
            GameObject* dropped = *(GameObject**)payload->Data;
            if (dropped != gameObject) {
                if (dropped->parent) {
                    auto& siblings = dropped->parent->children;
                    siblings.erase(std::remove(siblings.begin(), siblings.end(), dropped), siblings.end());
                }
                dropped->parent = gameObject;
                gameObject->children.push_back(dropped);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Popup context menu.
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Rename")) {
            renamingGameObject = gameObject;
            strncpy_s(renameBuffer, gameObject->name.c_str(), sizeof(renameBuffer) - 1);
            renameBuffer[sizeof(renameBuffer) - 1] = '\0';
            shouldOpenRenamePopup = true;
        }
        if (ImGui::MenuItem("Delete")) {
            if (gameObject->parent) {
                auto& siblings = gameObject->parent->children;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), gameObject), siblings.end());
            }
            delete gameObject;
            if (selected == gameObject)
                selected = nullptr;
            ImGui::EndPopup();
            if (open)
                ImGui::TreePop();
            return;
        }
        ImGui::EndPopup();
    }

    if (open) {
        for (GameObject* child : gameObject->children) {
            ShowGameObjectNode(child, selected);
        }
        ImGui::TreePop();
    }
}

void DrawHierarchyWindow(Scene* scene, GameObject*& selected) {
    if (ImGui::Begin("Hierarchy")) {
        if (ImGui::Button("Save")) {
            CurrentScene->SaveToFile(CurrentScene->sceneName + ".json");
        }

        if (ImGui::Button("Add Empty GameObject")) {
            GameObject* newObj = new GameObject("New GameObject " + std::to_string(emptyObjCount));
            emptyObjCount++;
            scene->AddGameObject(newObj);
        }
        for (GameObject* obj : scene->gameObjects) {
            if (obj->parent == nullptr) {
                ShowGameObjectNode(obj, selected);
            }
        }
    }
    ImGui::End();
}

int main()
{
    EditorState currentState = EditorState::Play;
    CurrentScene = std::make_unique<SampleScene>();

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed. Terminating program." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    Window = glfwCreateWindow(800, 800, "First OpenGL Window", nullptr, nullptr);
    if (Window == nullptr) {
        std::cerr << "GLFW window creation failed. Terminating program." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed. Terminating program." << std::endl;
        glfwTerminate();
        return -1;
    }

    InputManager::SetCallbacks(Window);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    editorCamera = new GameObject("Camera");
    editorCamera->transform.position = glm::vec3(0.0f, 500.0f, 0.0f);
    editorCamera->AddComponent<Camera>();
    editorCamera->AddComponent<CameraMovement>();

    frameBuffer = new FrameBuffer(800, 800);
    frameBuffer->Initialize();
    editorFrameBuffer = new FrameBuffer(800, 800);
    editorFrameBuffer->Initialize();

    CurrentScene->InitialSetup(Window);
    editorCamera->Start();

    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
        InputManager::Instance().Update();

        switch (currentState) {
        case EditorState::Play:
            CurrentScene->Update();
            break;
        case EditorState::Pause:
            break;
        case EditorState::Stop:
            switchScene(SceneType::Game);
            currentState = EditorState::Play;
            break;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // DockSpace
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
            ImGui::End();
        }

        // Scene View Window
        ImGui::Begin("Scene View");
        {
            editorCamera->Update(0.016f);
            ImVec2 imageSize(800, 600);
            CurrentScene->Render(editorFrameBuffer, editorCamera->GetComponent<Camera>());
            ImGui::Image((ImTextureID)(intptr_t)editorFrameBuffer->GetTextureID(), imageSize);
        }
        ImGui::End();

        // Game View Window
        ImGui::Begin("Game View");
        {
            if (ImGui::Button("Play")) {
                currentState = EditorState::Play;
            }
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                currentState = EditorState::Pause;
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                currentState = EditorState::Stop;
            }
            ImVec2 imageSize(800, 800);
            editorCamera->Render(editorCamera->GetComponent<Camera>());
            CurrentScene->Render(frameBuffer);
            ImGui::Image((ImTextureID)frameBuffer->GetTextureID(), imageSize);
            ImGui::Text("This is where the scene is rendered.");
        }
        ImGui::End();

        // Inspector Window - Updated to call inspector logic from selectedGameObject
        ImGui::Begin("Inspector");
        {
            if (selectedGameObject) {
                selectedGameObject->OnInspectorGUI();
            }
            else {
                ImGui::Text("Nothing selected.");
            }
        }
        ImGui::End();

        // Hierarchy Window
        DrawHierarchyWindow(CurrentScene.get(), selectedGameObject);

        // Rename Popup
        if (shouldOpenRenamePopup) {
            ImGui::OpenPopup("Rename GameObject");
            shouldOpenRenamePopup = false;
        }
        if (ImGui::BeginPopupModal("Rename GameObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("New Name", renameBuffer, IM_ARRAYSIZE(renameBuffer));
            if (ImGui::Button("OK")) {
                if (renamingGameObject) {
                    renamingGameObject->name = std::string(renameBuffer);
                    renamingGameObject = nullptr;
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                renamingGameObject = nullptr;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Project Window
        ImGui::Begin("Project");
        {
            ImGui::Text("Project panel");
        }
        ImGui::End();

        // Console Window
        ImGui::Begin("Console");
        {
            ImGui::Text("Console panel");
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Window);
    }

    delete frameBuffer;
    delete editorFrameBuffer;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
