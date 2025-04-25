#define GLM_ENABLE_EXPERIMENTAL
#include <memory>
#include <iostream>

#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "SampleScene.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "CameraMovement.h"
#include "InputManager.h"
#include "UIHelpers.h"

std::unique_ptr<SampleScene> editScene;
std::unique_ptr<SampleScene> runtimeScene;

GLFWwindow* Window = nullptr;
FrameBuffer* editorFrameBuffer = nullptr;
FrameBuffer* gameFrameBuffer = nullptr;
GameObject* editorCamera = nullptr;
GameObject* selectedGameObject = nullptr;

int main() {
    // — GLFW / OpenGL init —
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    Window = glfwCreateWindow(800, 800, "Editor", nullptr, nullptr);
    glfwMakeContextCurrent(Window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    glEnable(GL_MULTISAMPLE);

    InputManager::SetCallbacks(Window);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // — ImGui init —
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    UIHelpers::InitializeUI();

    // — Build our edit scene once —
    editScene = std::make_unique<SampleScene>();
    editScene->InitialSetup(Window);

    // — Editor camera (for gizmos/hierarchy) —
    editorCamera = new GameObject("EditorCamera");
    editorCamera->transform.position = glm::vec3(0, 500, 0);
    editorCamera->AddComponent<Camera>();
    editorCamera->AddComponent<CameraMovement>();

    // — Framebuffers for Scene / Game views —
    editorFrameBuffer = new FrameBuffer(800, 800);
    editorFrameBuffer->Initialize();
    gameFrameBuffer = new FrameBuffer(800, 800);
    gameFrameBuffer->Initialize();

    // — Editor state —
    EditorState state = EditorState::Stop;
    double      lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(Window)) {
        // — Poll & timing —
        glfwPollEvents();
        InputManager::Instance().Update();
        double now = glfwGetTime();
        float  dt = float(now - lastTime);
        lastTime = now;

        // — Play / Stop logic —
        if (state == EditorState::Play) {
            // on transition into Play
            if (!runtimeScene) {
                // 1) snapshot editScene to JSON
                constexpr const char* TMP = "TempScene.json";
                editScene->SaveToFile(TMP);

                // 2) build fresh runtimeScene
                runtimeScene = std::make_unique<SampleScene>();
                runtimeScene->InitialSetup(Window);
                runtimeScene->LoadFromFile(TMP);
                runtimeScene->Start();
            }
            // 3) drive only runtimeScene update
            runtimeScene->Update();
        }
        else {
            // leave editScene untouched, destroy runtime
            runtimeScene.reset();
        }

        // — Start new ImGui frame —
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // — Dockspace & windows —
        UIHelpers::ShowDockSpace();

        // 1) Scene View: always editScene + editorCamera
        UIHelpers::DrawSceneViewWindow(
            editorFrameBuffer,
            editorCamera,
            editScene.get(),
            selectedGameObject,
            dt);

        // 2) Game View: runtimeScene when playing, editScene otherwise
        UIHelpers::DrawGameViewWindow(
            gameFrameBuffer,
            editorCamera, /* you can swap in a dedicated in‐game camera too */
            (state == EditorState::Play ? runtimeScene.get() : editScene.get()),
            state,
            dt);

        // rest of your panels:
        UIHelpers::DrawInspectorWindow(selectedGameObject);
        UIHelpers::DrawHierarchyWindow(editScene.get(), selectedGameObject);
        UIHelpers::DrawProjectWindow();
        UIHelpers::DrawDebugWindow(nullptr);

        // — Render ImGui —
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Window);
    }

    // — Cleanup —
    delete editorFrameBuffer;
    delete gameFrameBuffer;
    delete editorCamera;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
