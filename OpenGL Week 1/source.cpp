/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : source.cpp
Description : Main entry point for the OpenGL application. Initializes GLFW, GLEW, sets up the main loop, all models, etc.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/




#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderLoader.h"
#include "Camera.h"
#include "Texture.h"
#include "InstanceMeshModel.h"
#include "InputManager.h"

// Define a GLFW window pointer
GLFWwindow* Window = nullptr;

// Define model pointers
MeshModel* model = nullptr;
MeshModel* skybox = nullptr;
InstanceMeshModel* instanceModel = nullptr;
InputManager* inputs = nullptr;

// Define a camera object
Camera camera;

// Define program IDs for shaders
GLuint Program_Texture = 0;
GLuint Program_instanceTexture = 0;

// Define variables for movement
double currentFrame = 0;
double lastFrame = currentFrame;
double deltaTime;
float modelSpeed = 100.0f;

// Define textures
Texture ancientTex;
Texture scifiTex;
Texture skyboxTex;

// Function prototypes
void InitialSetup();
void Update();
void Render();

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed. Terminating program." << std::endl;
        return -1;
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

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
    inputs = new InputManager(&camera);
    inputs->SetKeyCallback(Window);
    inputs->SetCursorPosCallback(Window);

    // Set cursor mode
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Perform initial setup
    InitialSetup();

    // Run the main loop
    while (!glfwWindowShouldClose(Window))
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

    // Clean up
    delete model;
    delete skybox;
    delete instanceModel;
    delete inputs;

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

// Function to perform initial setup
void InitialSetup()
{
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera.InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load shaders
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");

    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");

    // Initialize models
    glm::vec3 position(0.0f, -100.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);

    model = new MeshModel(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    model->SetTexture(ancientTex.GetId());
    model->SetShader(Program_Texture);

    skybox = new MeshModel(glm::vec3(0), glm::vec3(0), glm::vec3(500.0f), "Resources/Models/Sphere.obj");
    skybox->SetTexture(skyboxTex.GetId());
    skybox->SetShader(Program_Texture);

    instanceModel = new InstanceMeshModel(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Env_Tree_Palm_01.obj");
    instanceModel->SetTexture(ancientTex.GetId());
    instanceModel->SetShader(Program_instanceTexture);

    // Add instances at random positions
    for (int i = 0; i < 1000; i++) {
        glm::vec3 randomPosition = glm::vec3(rand() % 1000 - 100, -5.0f, rand() % 1000 - 100);
        glm::vec3 randomRotation = glm::vec3((rand() % 21) - 10);
        glm::vec3 randomScale = glm::vec3(((rand() % 10 + 5) / 200.0f));
        instanceModel->AddInstance(randomPosition, randomRotation, randomScale);
    }
}

// Function to update the scene
void Update()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    model->Update(deltaTime);

    // Player movement
    glm::vec3 moveDir(0.0f);

    // Forward and backward movement
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
        moveDir += camera.m_orientation;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
        moveDir -= camera.m_orientation;
    }

    // Left and right movement
    glm::vec3 right = glm::normalize(glm::cross(camera.m_orientation, camera.m_up));
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
        moveDir -= right;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
        moveDir += right;
    }

    // Up and down movement
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS) {
        moveDir += up;
    }
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS) {
        moveDir -= up;
    }

    // Normalize the movement direction
    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir);
    }

    // Apply movement to the model
    model->SetPosition(model->GetPosition() + moveDir * modelSpeed * float(deltaTime));
}

// Function to render the scene
void Render()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render skybox
    skybox->BindTexture();
    glUseProgram(Program_Texture);
    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    glCullFace(GL_FRONT);
    skybox->Render();
    glCullFace(GL_BACK);

    // Render model
    model->BindTexture();
    glUseProgram(Program_Texture);
    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    model->Render();

    // Render instance model
    instanceModel->BindTexture();
    glUseProgram(Program_instanceTexture);
    camera.Matrix(0.01f, 1000.0f, Program_instanceTexture, "CameraMatrix");
    instanceModel->Render();

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    // Swap buffers
    glfwSwapBuffers(Window);
}
