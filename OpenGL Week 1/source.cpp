
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderLoader.h"
//#include "VertexObject.h"
#include "Camera.h"
#include "MeshModel.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// Define properties for the first quad model
glm::vec3 quadModelPos1 = glm::vec3(-2.0f, -100.0f, -20.0f);
float quadModelRot1 = 0.0f;
glm::vec3 quadModelScl1 = glm::vec3(0.05f, 0.05f, 0.05f);


// Define program IDs for shaders
GLuint Program_VertexColor = 0;
GLuint Program_Texture = 0;
GLuint Program_AnimatedTexture = 0;

glm::mat4 ModelMat;

// Define variables for camera movement and animation
float CameraMovement = 0;
GLfloat quadPositionList1[] = { -0.5f, 0.5f, 0.0f, /**/ 1.0f, 0.0f, 0.0f, /**/ 0.0f, 4.0f,
                                0.5f, 0.5f, 0.0f, /**/ 0.0f, 1.0f, 0.0f, /**/ 0.0f, 0.0f,
                                0.5f, -0.5f, 0.0f, /**/ 1.0f, 0.0f, 1.0f, /**/ 4.0f, 0.0f,
                                -0.5f, -0.5f, 0.0f, /**/ 0.0f, 1.0f, 1.0f, /**/ 4.0f, 4.0f,
                                -0.5f, 0.5f, 1.0f, /**/ 1.0f, 0.0f, 0.0f, /**/ 0.0f, 4.0f,
                                0.5f, 0.5f, 1.0f, /**/ 0.0f, 1.0f, 0.0f, /**/ 0.0f, 0.0f,
                                0.5f, -0.5f, 1.0f, /**/ 1.0f, 0.0f, 1.0f, /**/ 4.0f, 0.0f,
                                -0.5f, -0.5f, 1.0f, /**/ 0.0f, 1.0f, 1.0f, /**/ 4.0f, 4.0f,
};


GLuint quadIndexesList[] = {
        //front
        2, 1, 0,
        0, 3, 2,
        //back
        4, 5, 6,
        6, 7, 4,
        //top
        5, 4, 0,
        0, 1, 5,
        //bot
        3, 7, 6,
        6, 2, 3,
        //right
        6, 5, 1,
        1, 2, 6,
        //left
        0, 4, 7,
        7, 3, 0       
};


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

/*
    Quad1.InitTexture("Resources/Textures/Alien.png");
    Quad1.setVBO(quadPositionList1, sizeof(quadPositionList1));
    Quad1.setVAO();
    Quad1.setEBO(quadIndexesList, sizeof(quadIndexesList));   
    
    
    Quad2.InitTexture("Resources/Textures/Coconut.png");
    Quad2.setVBO(quadPositionList1, sizeof(quadPositionList1));
    Quad2.setVAO();
    Quad2.setEBO(quadIndexesList, sizeof(quadIndexesList));
*/
    stbi_set_flip_vertically_on_load(true);
    imageData = stbi_load("Textures/PolygonAncientWorlds_Statue_01.png", &imageWidth, &imageHeight, &imageComponents, 0);
    if (imageData == nullptr) {
        std::cerr << "Failed to load image: " << std::endl;
        // Handle the error or return from the function.
    }
    // Generate a new texture and bind it.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Determine the number of color components in the image.
    GLint LoadedComponents = (imageComponents == 4) ? GL_RGBA : GL_RGB;
    // Load the image data into the texture.
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, imageWidth, imageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, imageData);
    // Generate mipmaps for the texture.
    glGenerateMipmap(GL_TEXTURE_2D);
    // Free the image data and unbind the texture.
    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Enable blending for the texture.
    glEnable(GL_BLEND);
    model = new MeshModel("Models/AncientEmpire/SM_Prop_Statue_01.obj");
   // model->InitTexture("Textures/PolygonAncientWorlds_Statue_01.png");
   // model->setShader(Program_Texture);
    
    // Combine the matrices to form the model matrix.
    ModelMat = glm::scale(glm::identity<glm::mat4>(), quadModelScl1) * glm::translate(glm::identity<glm::mat4>(), quadModelPos1) * glm::rotate(glm::identity<glm::mat4>(), glm::radians(quadModelRot1), glm::vec3(0.0f, 0.0f, 1.0f));;
   
}

// Function to update the scene
void Update()
{ 
    // Calculate delta time
    currentFrame = glfwGetTime();
    deltaTime = (currentFrame - lastFrame) * SpinSpeed;
    lastFrame = currentFrame;
/*
 
    Quad1.defineModelMatrix(quadModelPos1, quadModelRot1, quadModelScl1);
    Quad2.defineModelMatrix(quadModelPos2, quadModelRot2, quadModelScl2);
    */
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

  /*  // Render Quad1
    Quad1.setShader(Program_Texture);
    Quad1.setTexture(MIRRORED);
    glUniform1i(glGetUniformLocation(Program_Texture, "Texture0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(Program_Texture, "ModelMat"), 1, GL_FALSE, &Quad1.ModelMat[0][0]);
    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    Quad1.draw(sizeof(quadIndexesList));

    // Render Quad2
    Quad2.setShader(Program_Texture);
    Quad2.setTexture(MIRRORED);
    glUniform1i(glGetUniformLocation(Program_Texture, "Texture0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(Program_Texture, "ModelMat"), 1, GL_FALSE, &Quad2.ModelMat[0][0]);
    camera.Matrix(0.01f, 1000.0f, Program_Texture, "CameraMatrix");
    Quad2.draw(sizeof(quadIndexesList));*/


    // Activate the first texture unit.
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set texture filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set texture wrapping mode based on the specified mode.
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
   
    // Set the uniform for the texture in the shader program.
    glUniform1i(glGetUniformLocation(Program_Texture, "Texture0"), 0);



    glUseProgram(Program_Texture);
    glUniform1i(glGetUniformLocation(Program_Texture, "Texture0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(Program_Texture, "ModelMat"), 1, GL_FALSE, &ModelMat[0][0]);
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
