#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <iostream>

// GLFW / GLEW
#include <glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


// Your engine headers
#include "SampleScene.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "CameraMovement.h"
#include "InputManager.h"
#include "UIHelpers.h"   // brings in EditorState

std::unique_ptr<Scene> CurrentScene;
GLFWwindow* Window = nullptr;
FrameBuffer* editorFrameBuffer = nullptr;
FrameBuffer* frameBuffer = nullptr;
GameObject* editorCamera = nullptr;
GameObject* selectedGameObject = nullptr;
int emptyObjCount = 0;
static bool showDebugWindow = true;

enum class SceneType { Game };

void switchScene(SceneType sceneType) {
    if (sceneType == SceneType::Game) {
        CurrentScene = std::make_unique<SampleScene>();
        std::cout << "Switched to SampleScene\n";
        CurrentScene->InitialSetup(Window);
    }
}

int main()
{
    // --- GLFW init ---
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // --- Window & context ---
    Window = glfwCreateWindow(800, 800, "First OpenGL Window", nullptr, nullptr);
    if (!Window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    // --- GLEW init (after context!) ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        glfwTerminate();
        return -1;
    }
    glEnable(GL_MULTISAMPLE);

    // --- Input setup ---
    InputManager::SetCallbacks(Window);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // --- ImGui setup ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    UIHelpers::InitializeUI();

    // --- Scene & camera setup ---
    EditorState currentState = EditorState::Play;
    CurrentScene = std::make_unique<SampleScene>();

    editorCamera = new GameObject("Camera");
    editorCamera->transform.position = glm::vec3(0.0f, 500.0f, 0.0f);
    editorCamera->AddComponent<Camera>();
    editorCamera->AddComponent<CameraMovement>();

    // --- Framebuffers ---
    frameBuffer = new FrameBuffer(800, 800);
    frameBuffer->Initialize();
    editorFrameBuffer = new FrameBuffer(800, 800);
    editorFrameBuffer->Initialize();

    // --- Initial scene load ---
    CurrentScene->InitialSetup(Window);
    editorCamera->Start();

    // --- Main loop timing ---
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(Window)) {
        // Poll + input
        glfwPollEvents();
        InputManager::Instance().Update();

        // Compute deltaTime
        double now = glfwGetTime();
        float  deltaTime = static_cast<float>(now - lastTime);
        lastTime = now;

        // Scene update
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

        // Start ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI
        UIHelpers::ShowDockSpace();
        UIHelpers::DrawSceneViewWindow(editorFrameBuffer,
            editorCamera,
            CurrentScene.get(),
            selectedGameObject,
            deltaTime);
        UIHelpers::DrawGameViewWindow(frameBuffer,
            editorCamera,
            CurrentScene.get(),
            currentState,
            deltaTime);
        UIHelpers::DrawInspectorWindow(selectedGameObject);
        UIHelpers::DrawHierarchyWindow(CurrentScene.get(), selectedGameObject);
        UIHelpers::DrawProjectWindow();
        UIHelpers::DrawDebugWindow(&showDebugWindow);

        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Window);
    }

    // Cleanup
    delete frameBuffer;
    delete editorFrameBuffer;
    delete editorCamera;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
