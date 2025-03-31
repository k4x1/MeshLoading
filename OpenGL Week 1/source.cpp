#include <memory>
#include <iostream>
#include "SampleScene.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "FrameBuffer.h"
// Function prototypes
std::unique_ptr<Scene> CurrentScene;
GLFWwindow* Window = nullptr;
Camera* camera = new Camera();
InputManager* inputs = nullptr;
FrameBuffer* frameBuffer;
void switchScene(InputManager::SceneType sceneType) {
    switch (sceneType) {
    case InputManager::SceneType::Game:
        CurrentScene = std::make_unique<SampleScene>();
        std::cout << "Shadow Scene" << std::endl;
        break;
    default:
        break;
    }
    CurrentScene->InitialSetup(Window, camera);
    frameBuffer = CurrentScene->GetFrameBuffer();
}
enum class EditorState {
    Play,      
    Pause,      
    Stop        
};
int main()
{
    EditorState currentState = EditorState::Play;

    CurrentScene = std::make_unique<SampleScene>();

    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed. Terminating program." << std::endl;
        return -1;
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    // Create a window
    Window = glfwCreateWindow(800, 800, "First OpenGL Window", NULL, NULL);
    if (Window == NULL)
    {
        std::cerr << "GLFW window creation failed. Terminating program." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);


    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed. Terminating program." << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set input callbacks
    inputs = new InputManager(camera, CurrentScene.get());
    inputs->SetCursorPosCallback(Window);

    // Set cursor mode
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");


    // Perform initial setup
    CurrentScene->InitialSetup(Window, camera);
    frameBuffer = CurrentScene->GetFrameBuffer();
    // Run the main loop
    while (!glfwWindowShouldClose(Window))
    {
        // Process events
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        ImGui::Begin("Scene View");
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
            ImGui::Image((ImTextureID)frameBuffer->GetTextureID(), imageSize);
            ImGui::Text("This is where the scene is rendered.");
        }

        ImGui::End();

        ImGui::Begin("Inspector");
        {
            ImGui::Text("Inspector panel");
        }
        ImGui::End();

        ImGui::Begin("Hierarchy");
        {
            ImGui::Text("Hierarchy panel (list of GameObjects)");
        }
        ImGui::End();

        ImGui::Begin("Project");
        {
            ImGui::Text("Project panel");
        }
        ImGui::End();

        ImGui::Begin("Console");
        {
            ImGui::Text("Console panel");
        }
        ImGui::End();

        // Process input
        inputs->ProcessInput(Window);
        if (inputs->sceneChanged) {
            switchScene(inputs->currentScene);
            inputs->sceneChanged = false;
        }
        // Update the scene
        switch (currentState) {
        case EditorState::Play:
            // Run simulation updates normally.
            CurrentScene->Update();
            break;
        case EditorState::Pause:
            // Skip updates, or update only UI elements.
            break;
        case EditorState::Stop:
           
            switchScene(InputManager::SceneType::Game);
            currentState = EditorState::Play;
            break;
        } 

        // Render the scene
        CurrentScene->Render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Window);
    }

    // Clean up
    delete inputs;
    delete camera;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // Terminate GLFW
    glfwTerminate();
    return 0;
}
