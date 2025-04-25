// main.cpp
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

constexpr float FIXED_DT = 1.0f / 60.0f;

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
    editorFrameBuffer = new FrameBuffer(800, 800); editorFrameBuffer->Initialize();
    gameFrameBuffer = new FrameBuffer(800, 800); gameFrameBuffer->Initialize();

    // — Editor state & timing —
    EditorState state = EditorState::Stop;
    double      lastTime = glfwGetTime();
    float       accumulator = 0.0f;

    while (!glfwWindowShouldClose(Window)) {
        // — Poll & timing —
        glfwPollEvents();
        InputManager::Instance().Update();
        double now = glfwGetTime();
        float  frameDt = float(now - lastTime);
        lastTime = now;
        accumulator += frameDt;

        // — Fixed‐step updates —
        while (accumulator >= FIXED_DT) {
            if (state == EditorState::Play && runtimeScene) {
                runtimeScene->FixedUpdate(FIXED_DT);
            }
            else {
                editScene->FixedUpdate(FIXED_DT);
            }
            accumulator -= FIXED_DT;
        }

        // — Variable‐timestep Update (for e.g. animation, editor UI) —
        if (state == EditorState::Play) {
            if (!runtimeScene) {
                editScene->SaveToFile("TempScene.json");
                runtimeScene = std::make_unique<SampleScene>();
                runtimeScene->InitialSetup(Window);
                runtimeScene->LoadFromFile("TempScene.json");
                runtimeScene->Start();
            }
            runtimeScene->Update(frameDt);
        }
        else {
            runtimeScene.reset();
            editScene->Update(frameDt);
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
            frameDt);

        // 2) Game View: runtimeScene when playing, editScene otherwise
        UIHelpers::DrawGameViewWindow(
            gameFrameBuffer,
            editorCamera,
            (state == EditorState::Play ? runtimeScene.get() : editScene.get()),
            state,
            frameDt);

        // — other panels —
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
