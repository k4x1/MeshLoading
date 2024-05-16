
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderLoader.h"
#include "Camera.h"
#include "MeshModel.h"

// Define a GLFW window pointer
GLFWwindow* Window = nullptr;

MeshModel* model;

GLuint texture;
unsigned char* imageData;
int imageWidth = 1024;
int imageHeight = 512;
int imageComponents;

// Define a camera object
Camera camera;

// Define program IDs for shaders
GLuint Program_VertexColor = 0;
GLuint Program_Texture = 0;
GLuint Program_AnimatedTexture = 0;

glm::mat4 ModelMat;

// Define variables for camera movement and animation
float CameraMovement = 0;

float CurrentTime = 0;

double currentFrame = 0;
double lastFrame = currentFrame;
double deltaTime;
float SpinSpeed = 100;

// Function to set up initial OpenGL settings and load shaders
void InitialSetup()
{
    // Set clear color and viewport
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera.InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load shaders
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_AnimatedTexture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/AnimatedTexture.frag");
    Program_VertexColor = ShaderLoader::CreateProgram("Resources/Shaders/VertexColor.vert", "Resources/Shaders/VertexColor.frag");

    glm::vec3 position(0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);
    model = new MeshModel(position, rotation, scale, "Models/AncientEmpire/SM_Prop_Statue_01.obj");
    model->InitTexture("Textures/PolygonAncientWorlds_Statue_01.png");
    model->SetShader(Program_Texture);
}

// Function to update the scene
void Update()
{ 
    // Calculate delta time
    currentFrame = glfwGetTime();
    deltaTime = (currentFrame - lastFrame) * SpinSpeed;
    lastFrame = currentFrame;
    const float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    // Update camera movement
    CameraMovement += deltaTime;
   //camera.MoveCamera(glm::vec3(camera.Position.x + glm::sign(glm::sin(CameraMovement / 100)) / 10, camera.Position.y, camera.Position.z));
    camera.view = glm::lookAt(glm::vec3(camX, 0, camZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    model->Update(deltaTime);
   
}

// Function to render the scene
void Render()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Activate and Bind the first texture unit.
    model->BindTexture();
   
    // Set the uniform for the texture in the shader program.
    glUseProgram(Program_Texture);

    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    
    model->Render();


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

    // Perform initial setup
    InitialSetup();

    // Run the main loop
    while (glfwWindowShouldClose(Window) == false)
    {
        // Process events
        glfwPollEvents();

        // Update the scene
        Update();

        // Render the scene
        Render();
    }
    delete model;
    // Terminate GLFW
    glfwTerminate();
    return 0;
}
