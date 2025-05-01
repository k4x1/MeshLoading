#include "All.h"
#include "EnginePluginAPI.h" 
#include "PluginLoader.h"

constexpr float FIXED_DT = 1.0f / 60.0f;

std::unique_ptr<SampleScene> runtimeScene;

GLFWwindow* Window = nullptr;
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


    PluginLoader componentLoader("GameComponents.dll");
    if (!componentLoader.Load()) {
        return -1;
    }


    double      lastTime = Engine::GetTime();
    float       accumulator = 0.0f;
    runtimeScene = std::make_unique<SampleScene>();
    runtimeScene->LoadFromFile("Assets/scene.json");
    runtimeScene->InitialSetup(Window, false);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    runtimeScene->Start();
    while (!glfwWindowShouldClose(Window)) {

        InputManager::Instance().Update();
        Engine::PollEvents();
        double now = Engine::GetTime();
        float  frameDt = float(now - lastTime);
        lastTime = now;
        accumulator += frameDt;
        if (runtimeScene) {
            while (accumulator >= FIXED_DT) {
                runtimeScene->FixedUpdate(FIXED_DT);
                accumulator -= FIXED_DT;
            }
        }
               


        runtimeScene->Update(frameDt);

        runtimeScene->Render(nullptr, nullptr);
        Engine::SwapBuffers(Window);
    }

    Engine::TerminateGLFW();
    return 0;
}
