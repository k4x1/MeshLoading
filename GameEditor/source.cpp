#include "All.h"
#include "EnginePluginAPI.h" 
#include "PluginLoader.h"

constexpr float FIXED_DT = 1.0f / 60.0f;

std::unique_ptr<SampleScene> editScene;
std::unique_ptr<SampleScene> runtimeScene;

GLFWwindow* Window = nullptr;
FrameBuffer* editorFrameBuffer = nullptr;
FrameBuffer* gameFrameBuffer = nullptr;
GameObject* editorCamera = nullptr;
GameObject* selectedGameObject = nullptr;
bool played = false;
int main() {
   
    if (!Engine::InitGLFW())
        return -1;
    Engine::SetGLFWWindowHints(4, 6, 4);
    GLFWwindow* Window = Engine::CreateGLFWWindow(1920, 1080, "Editor");
    Engine::MakeContextCurrent(Window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    glEnable(GL_MULTISAMPLE);
   
    InputManager::Instance().SetCallbacks(Window);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    

    UIHelpers::Init(Window, "#version 460");
    PluginLoader componentLoader("GameComponents.dll");
    if (!componentLoader.Load()) {
        return -1;
    }
    UIHelpers::InitializeUI();

    editScene = std::make_unique<SampleScene>();
    editScene->InitialSetup(Window, true);

    editorCamera = new GameObject("EditorCamera");
    editorCamera->transform.position = glm::vec3(0, 500, 0);
    editorCamera->AddComponent<Camera>();
    editorCamera->AddComponent<CameraMovement>();

    editorFrameBuffer = new FrameBuffer(1920, 1080); editorFrameBuffer->Initialize();
    gameFrameBuffer = new FrameBuffer(1920, 1080); gameFrameBuffer->Initialize();

    EditorState state = EditorState::Stop;
    double      lastTime = Engine::GetTime();
    float       accumulator = 0.0f;

    while (!glfwWindowShouldClose(Window)) {

        InputManager::Instance().Update();
        Engine::PollEvents();
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
            played = true;
            if (!runtimeScene) {
                editScene->SaveToFile("TempScene.json");
                Physics::PhysicsEngine::Instance().Reset();
                runtimeScene = std::make_unique<SampleScene>();
                runtimeScene->LoadFromFile("Assets/TempScene.json");
                runtimeScene->InitialSetup(Window, false);

                runtimeScene->Start();
            }
            runtimeScene->Update(frameDt);
        }
        else if(played){
            selectedGameObject = nullptr;   
            runtimeScene.reset();
            Physics::PhysicsEngine::Instance().Reset();
            played = false;
        }

        UIHelpers::NewFrame();
        UIHelpers::ShowDockSpace();
        Scene* activeScene = (state == EditorState::Play && runtimeScene)
            ? runtimeScene.get()
            : editScene.get();
        UIHelpers::DrawSceneViewWindow(
            editorFrameBuffer,
            editorCamera,
            activeScene,
            selectedGameObject,
            frameDt);

        UIHelpers::DrawGameViewWindow(
            gameFrameBuffer, 
            activeScene,
            state);

        UIHelpers::DrawInspectorWindow(selectedGameObject);
        UIHelpers::DrawHierarchyWindow(activeScene, selectedGameObject, editorCamera);
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
