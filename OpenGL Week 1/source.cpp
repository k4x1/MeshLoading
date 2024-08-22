#include <memory>
#include "GameScene.h"
#include "SkyboxScene.h"

// Function prototypes
std::unique_ptr<Scene> CurrentScene;
GLFWwindow* Window = nullptr;
Camera* camera = new Camera();
InputManager* inputs = nullptr;

int main()
{
    CurrentScene = std::make_unique<SkyboxScene>();

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
    inputs = new InputManager(camera);
    inputs->SetCursorPosCallback(Window);

    // Set cursor mode
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Perform initial setup
    CurrentScene->InitialSetup(Window, camera);

    // Run the main loop
    while (!glfwWindowShouldClose(Window))
    {
        // Process events
        glfwPollEvents();

        // Process input
        inputs->ProcessInput(Window);

        // Update the scene
        CurrentScene->Update();

        // Render the scene
        CurrentScene->Render();
    }

    // Clean up
    delete inputs;
    delete camera;

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
