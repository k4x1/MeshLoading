 
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderLoader.h"
#include "Camera.h"
//#include "MeshModel.h"
#include "InstanceMeshModel.h"
#include "InputManager.h"
// Define a GLFW window pointer
GLFWwindow* Window = nullptr;

MeshModel* model = nullptr;
InstanceMeshModel* instanceModel = nullptr;
InputManager* inputs = nullptr;


// Define a camera object
Camera camera;

// Define program IDs for shaders
GLuint Program_Texture = 0;
GLuint Program_instanceTexture = 0;


// Define variables for camera movement and animation
float CameraMovement = 0;

float CurrentTime = 0;

double currentFrame = 0;
double lastFrame = currentFrame;
double deltaTime;

float SpinSpeed = 100;


void InitialSetup()
{
    // Set clear color and viewport
    inputs = new InputManager(&camera);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera.InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load shaders
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");

    glm::vec3 position(0.0f, -100.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);

    model = new MeshModel(position, rotation, scale, "Models/AncientEmpire/SM_Prop_Statue_01.obj");
    model->LoadModel();
    model->InitTexture("Textures/PolygonAncientWorlds_Texture_01_A.png");
    model->SetShader(Program_Texture);

    instanceModel = new InstanceMeshModel(position, rotation, scale, "Models/AncientEmpire/SM_Env_Tree_Palm_01.obj");
    instanceModel->InitTexture("Textures/PolygonAncientWorlds_Texture_01_B.png");
    instanceModel->SetShader(Program_instanceTexture);

    // Add instances at random positions
    for (int i = 0; i < 100; i++) {
        glm::vec3 randomPosition = glm::vec3(rand() % 200 - 100, -5.0f, rand() % 200 - 100);
        instanceModel->AddInstance(randomPosition, rotation, glm::vec3(0.025));
    }
}

// Function to update the scene
void Update()
{ 
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    model->Update(deltaTime);
    instanceModel->Update(deltaTime);
   
}

// Function to render the scene
void Render()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Activate and Bind the first texture unit.
    model->BindTexture();
    instanceModel->BindTexture();

    // Set the uniform for the texture in the shader program.
    glUseProgram(Program_Texture);
    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    model->Render();

    glUseProgram(Program_instanceTexture);
    camera.Matrix(0.01f, 1000.0f, Program_instanceTexture, "CameraMatrix");
    instanceModel->Render();

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << error << std::endl;
    }

    // Swap buffers
    glfwSwapBuffers(Window);
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // Create a window
    Window = glfwCreateWindow(800, 800, "First OpenGL Window", NULL, NULL);
    if (Window == NULL)
    {
        std::cout << "GLFW failed to initialize properly. Terminating program." << std::endl;
        system("pause");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to initialize properly. Terminating program." << std::endl;
        system("pause");
        glfwTerminate();
        return -1;
    }
    inputs->SetCursorPosCallback(Window);

    // Set cursor mode
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Perform initial setup
    InitialSetup();
   


    // Run the main loop
    while (glfwWindowShouldClose(Window) == false)
    {
        // Process events
        glfwPollEvents();

        // Process input
         inputs->ProcessInput(Window);

        // Update the scene
        Update();

        // Render the scene
        Render();
    }
    delete model;
    delete instanceModel;
    delete inputs;
    // Terminate GLFW
    glfwTerminate();
    return 0;
}
