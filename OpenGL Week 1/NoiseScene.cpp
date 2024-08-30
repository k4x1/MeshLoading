#include "NoiseScene.h"

void NoiseScene::InitialSetup(GLFWwindow* _window, Camera* _camera) {
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load skybox shader
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");

    // Initialize skybox textures
    faces = {
           "Resources/Textures/skybox/Right.png",
           "Resources/Textures/skybox/Left.png",
           "Resources/Textures/skybox/Top.png",
           "Resources/Textures/skybox/Bottom.png",
           "Resources/Textures/skybox/Back.png",
           "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);
}

void NoiseScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process input (if any)
}

void NoiseScene::Render() {
    // Clear the color buffer
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the skybox
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    // Swap buffers
    glfwSwapBuffers(Window);
}

int NoiseScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        Update();
        Render();
        glfwPollEvents();
    }
    return 0;
}

NoiseScene::~NoiseScene() {
    delete skybox;
}
