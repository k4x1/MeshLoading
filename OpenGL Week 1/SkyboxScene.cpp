#include "SkyboxScene.h"
#include "HeightMapInfo.h"
#include "Texture.h"
#include <iostream>

void SkyboxScene::InitialSetup(GLFWwindow* _window, Camera* _camera) {
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load skybox shader
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    // Load terrain shader
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");

    // Initialize skybox textures
    faces = {
           "Resources/Textures/skybox/Right.png",
           "Resources/Textures/skybox/Left.png",
           "Resources/Textures/skybox/Bottom.png",
           "Resources/Textures/skybox/Top.png",
           "Resources/Textures/skybox/Back.png",
           "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    // Initialize input manager
    glDisable(GL_CULL_FACE);

    // Initialize terrain
    HeightMapInfo heightMapInfo = { "Resources/Heightmaps/heightmap.raw", 512, 512, 1.0f };
    std::vector<float> heightMap;
    if (LoadHeightMap(heightMapInfo, heightMap)) {
        std::vector<VertexStandard> vertices;
        std::vector<unsigned int> indices;
        BuildVertexData(heightMapInfo, heightMap, vertices);
        BuildIndexData(heightMapInfo, indices);
        ComputeNormals(vertices, indices);

        GLuint VAO, VBO, EBO;
        BuildEBO(vertices, indices, VAO, VBO, EBO);
        terrainVAO = VAO;
        terrainVBO = VBO;
        terrainEBO = EBO;
        this->indices = indices; // Store indices for rendering

        // Load terrain texture
        Texture terrainTexture;
        terrainTexture.InitTexture("Resources/Heightmaps/heightmap.jpg");
        this->terrainTexture = terrainTexture.GetId();
    }
}

void SkyboxScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process input

}
void SkyboxScene::Render() {
    // Clear the color buffer
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the skybox
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);

    // Render the terrain
    glUseProgram(Program_terrain);
    glBindVertexArray(terrainVAO);

    // Set uniforms for the terrain shader
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "view"), 1, GL_FALSE, glm::value_ptr(camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    // Set light and view position
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos"), 1, glm::value_ptr(glm::vec3(0.0f, 10.0f, 10.0f)));
    glUniform3fv(glGetUniformLocation(Program_terrain, "viewPos"), 1, glm::value_ptr(camera->m_position));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(glGetUniformLocation(Program_terrain, "objectColor"), 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);
    glUniform1i(glGetUniformLocation(Program_terrain, "texture1"), 0);

    // Draw the terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    // Swap buffers
    glfwSwapBuffers(Window);
}

int SkyboxScene::MainLoop() {
    //if (!glfwInit()) {
    //    std::cerr << "GLFW initialization failed. Terminating program." << std::endl;
    //    return -1;
    //}

    //// Set GLFW window hints
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    //glEnable(GL_MULTISAMPLE);

    //// Create a window
    //Window = glfwCreateWindow(800, 800, "Skybox Scene", NULL, NULL);
    //if (Window == NULL) {
    //    std::cerr << "GLFW window creation failed. Terminating program." << std::endl;
    //    glfwTerminate();
    //    return -1;
    //}
    //glfwMakeContextCurrent(Window);

    //// Initialize GLEW
    //if (glewInit() != GLEW_OK) {
    //    std::cerr << "GLEW initialization failed. Terminating program." << std::endl;
    //    glfwTerminate();
    //    return -1;
    //}

    //inputs = new InputManager(camera);

    //inputs->SetCursorPosCallback(Window);

    //// Set cursor mode
    //glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //// Perform initial setup
    //InitialSetup();

    //// Run the main loop
    //while (!glfwWindowShouldClose(Window)) {
    //    // Process events
    //    glfwPollEvents();

    //    // Update the scene
    //    inputs->ProcessInput(Window);

    //    Update();

    //    // Render the scene
    //    Render();
    //}

    //// Clean up
    //delete skybox;

    //// Terminate GLFW
    //glfwTerminate();
    return 0;
}

SkyboxScene::~SkyboxScene() {
    delete skybox;
    delete inputs;
}
