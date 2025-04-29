#include "All.h"
#include "EnginePluginAPI.h" 

constexpr float FIXED_DT = 1.0f / 60.0f;

std::unique_ptr<SampleScene> editScene;
std::unique_ptr<SampleScene> runtimeScene;

GLFWwindow* Window = nullptr;
FrameBuffer* editorFrameBuffer = nullptr;
FrameBuffer* gameFrameBuffer = nullptr;
GameObject* editorCamera = nullptr;
GameObject* selectedGameObject = nullptr;

int main() {
    if (!Engine::InitGLFW())
        return -1;
    Engine::SetGLFWWindowHints(4, 6, 4);
    GLFWwindow* Window = Engine::CreateGLFWWindow(800, 800, "Editor");
    Engine::MakeContextCurrent(Window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    glEnable(GL_MULTISAMPLE);

    InputManager::Instance().SetCallbacks(Window);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

 
    UIHelpers::Init(Window, "#version 460");
    UIHelpers::InitializeUI();

    editScene = std::make_unique<SampleScene>();
    editScene->InitialSetup(Window, true);

    editorCamera = new GameObject("EditorCamera");
    editorCamera->transform.position = glm::vec3(0, 500, 0);
    editorCamera->AddComponent<Camera>();
    editorCamera->AddComponent<CameraMovement>();

    editorFrameBuffer = new FrameBuffer(800, 800); editorFrameBuffer->Initialize();
    gameFrameBuffer = new FrameBuffer(800, 800); gameFrameBuffer->Initialize();

    EditorState state = EditorState::Stop;
    double      lastTime = Engine::GetTime();
    float       accumulator = 0.0f;

    while (!glfwWindowShouldClose(Window)) {
        Engine::PollEvents();
        InputManager::Instance().Update();
        double now = Engine::GetTime();
        float  frameDt = float(now - lastTime);
        lastTime = now;
        accumulator += frameDt;
        if (state == EditorState::Play && runtimeScene) {
            while (accumulator >= FIXED_DT) {
                runtimeScene->FixedUpdate(FIXED_DT);
                accumulator -= FIXED_DT;
            }
        }
        if (state == EditorState::Play) {
            if (!runtimeScene) {
                editScene->SaveToFile("Assets/TempScene.json");

                runtimeScene = std::make_unique<SampleScene>();
                runtimeScene->InitialSetup(Window, false);

                runtimeScene->LoadFromFile("Assets/TempScene.json");
                runtimeScene->Start();
            }
            runtimeScene->FixedUpdate(FIXED_DT);
            runtimeScene->Update(frameDt);
        }
        else {
            runtimeScene.reset();
        }

        UIHelpers::NewFrame();
        UIHelpers::ShowDockSpace();

        UIHelpers::DrawSceneViewWindow(
            editorFrameBuffer,
            editorCamera,
            editScene.get(),
            selectedGameObject,
            frameDt);


        GameObject* gameCamGO = (state == EditorState::Play && runtimeScene)
            ? runtimeScene->camera->owner
            : editScene->camera->owner;

        UIHelpers::DrawGameViewWindow(
            gameFrameBuffer,
            gameCamGO,
            (state == EditorState::Play ? runtimeScene.get() : editScene.get()),
            state,
            frameDt);

        UIHelpers::DrawInspectorWindow(selectedGameObject);
        UIHelpers::DrawHierarchyWindow(editScene.get(), selectedGameObject, editorCamera);
        UIHelpers::DrawProjectWindow();
        UIHelpers::DrawDebugWindow(nullptr);

        UIHelpers::Render();
        Engine::SwapBuffers(Window);
    }

    delete editorFrameBuffer;
    delete gameFrameBuffer;
    delete editorCamera;
    UIHelpers::Shutdown();
    Engine::TerminateGLFW();
    return 0;
}
