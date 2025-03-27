#include "SampleScene.h"
#include "ShaderLoader.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include "Skybox.h"
#include "FrameBuffer.h"
#include "ShadowMap.h"
#include "Light.h"
#include "HeightMapInfo.h"
#include "MeshRenderer.h"  // MeshRenderer now handles all model rendering
#include <fstream>
#include <iostream>
#include <gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>

//////////////////////////////////////
// Initialization & Setup Functions //
//////////////////////////////////////

void SampleScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    // Call the base Scene setup.
    Scene::InitialSetup(_window, _camera);
    Window = _window;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 0.0f));

    // Load shader programs for extra passes.
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");
    Program_color = ShaderLoader::CreateProgram("Resources/Shaders/Color.vert", "Resources/Shaders/Color.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
    shadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMapping.vert", "Resources/Shaders/ShadowMapping.frag");
    instanceShadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMappingInstance.vert", "Resources/Shaders/ShadowMapping.frag");
    mainRenderingShader = ShaderLoader::CreateProgram("Resources/Shaders/MainRendering.vert", "Resources/Shaders/MainRendering.frag");

    // Initialize textures.
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    // Setup skybox.
    faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    // Initialize models (each adds its own MeshRenderer component).
    InitializeModels();
    SetupLights();
    SetupTerrain();
    SetupQuad();

    // Create framebuffer for post-processing.
    m_FrameBuffer = new FrameBuffer(800, 800);

    // Start the scene (calls Start on all game objects).
    Start();
}

void SampleScene::Start() {
    Scene::Start();
}

//////////////////////////////////////
// Update Function                  //
//////////////////////////////////////

void SampleScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Let the base Scene update all game objects (MeshRenderer::Update is called as needed).
    Scene::Update();

    // Update spotlight from camera.
    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;

    // Cycle post-processing effects using TAB.
    static bool tabPressed = false;
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        currentEffect = static_cast<PostProcessEffect>((static_cast<int>(currentEffect) + 1) % 3);
    }
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }
}

void SampleScene::Render() {
    RenderShadowMap();

    RenderSceneExtras();

    Scene::Render();
    RenderPostProcessing();
}


//////////////////////////////////////
// Render Functions                 //
//////////////////////////////////////

// RenderShadowMap now iterates over all game objects with a MeshRenderer and calls RenderWithShader.
void SampleScene::RenderShadowMap() {
    // First shadow map pass (directional light 1).
    m_ShadowMap1->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::vec3 lightPos = dirLight1.direction * -700.0f;
    m_ShadowMap1->UpdateLightSpaceMatrix(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUseProgram(shadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));
    // Render all models using the shadow mapping shader.
    for (GameObject* obj : gameObjects) {
        MeshRenderer* mr = obj->GetComponent<MeshRenderer>();
        if (mr) {
            mr->Render(camera);
        }
    }
    m_ShadowMap1->Unbind();

    // Second shadow map pass (directional light 2).
    m_ShadowMap2->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    lightPos = dirLight2.direction * -700.0f;
    m_ShadowMap2->UpdateLightSpaceMatrix(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUseProgram(shadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));
    for (GameObject* obj : gameObjects) {
        MeshRenderer* mr = obj->GetComponent<MeshRenderer>();
        if (mr) {
            mr->Render(camera);
        }
    }
    m_ShadowMap2->Unbind();
}

// RenderSceneExtras handles skybox and terrain.
void SampleScene::RenderSceneExtras() {
    m_FrameBuffer->Bind();
    glViewport(0, 0, 800, 800);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Setup camera matrices.
    camera->Matrix(0.01f, 1000.0f, mainRenderingShader, "VPMatrix");
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    glm::mat4 projection = camera->m_projection;

    // Render skybox.
    skybox->Render(view, projection);

    // Render terrain.
    glUseProgram(Program_terrain);
    glBindVertexArray(terrainVAO);
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "view"), 1, GL_FALSE, glm::value_ptr(camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos1"), 1, glm::value_ptr(dirLight1.direction * -700.0f));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos2"), 1, glm::value_ptr(dirLight2.direction * -700.0f));
    glUniform3fv(glGetUniformLocation(Program_terrain, "viewPos"), 1, glm::value_ptr(camera->m_position));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
    glUniform3fv(glGetUniformLocation(Program_terrain, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "grassTexture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dirtTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "dirtTexture"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, stoneTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "stoneTexture"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snowTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "snowTexture"), 3);
    glActiveTexture(GL_TEXTURE4);
    m_ShadowMap1->BindTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(Program_terrain, "shadowMap1"), 4);
    glActiveTexture(GL_TEXTURE5);
    m_ShadowMap2->BindTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(Program_terrain, "shadowMap2"), 5);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    m_FrameBuffer->Unbind();

    // Prepare for post-processing.
    glViewport(0, 0, 800, 800);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

// RenderPostProcessing performs the full-screen quad pass.
void SampleScene::RenderPostProcessing() {
    glViewport(0, 0, 800, 800);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), static_cast<int>(currentEffect));
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    m_FrameBuffer->BindTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

//////////////////////////////////////
// Main Loop and Cleanup            //
//////////////////////////////////////

int SampleScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        Update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Extra passes.
        RenderShadowMap();
        RenderSceneExtras();
        // The base Scene::Render() iterates over game objects and calls their Render(camera),
        // which for models is handled by MeshRenderer.
        Scene::Render();
        RenderPostProcessing();
        glfwSwapBuffers(Window);
    }
    return 0;
}

void SampleScene::InitializeModels() {
    // Create a GameObject and attach a MeshRenderer.
    glm::vec3 position(0.0f, 500.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);
    mainModel = new GameObject("MainModel");
    // Attach a MeshRenderer with the model file, default shader, and texture.
    MeshRenderer* mainRenderer = mainModel->AddComponent<MeshRenderer>(
        position, rotation, scale,
        "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj",
        Program_Texture,
        ancientTex.GetId()
    );
    // Add the game object to the scene. MeshRenderer now fully controls model rendering.
    AddGameObject(mainModel);
}

void SampleScene::SetupLights() {
    m_ShadowMap1 = new ShadowMap();
    m_ShadowMap2 = new ShadowMap();

    dirLight1.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight1.color = glm::vec3(1.0f, 1.0f, 1.0f);

    dirLight2.direction = glm::vec3(0.3f, -1.0f, 0.2f);
    dirLight2.color = glm::vec3(0.8f, 0.8f, 0.8f);

    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;
    spotLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specularStrength = 1.0f;
    spotLight.attenuationConstant = 1.0f;
    spotLight.attenuationLinear = 0.014f;
    spotLight.attenuationExponent = 0.0007f;
    spotLight.innerCutoff = glm::cos(glm::radians(13.0f));
    spotLight.outerCutoff = glm::cos(glm::radians(15.0f));

    glDisable(GL_CULL_FACE);
}

void SampleScene::SetupTerrain() {
    HeightMapInfo heightMapInfo = { "Resources/Heightmaps/heightmap.raw", 512, 512, 1.0f };
    std::vector<float> heightMap;
    if (LoadHeightMap(heightMapInfo, heightMap)) {
        SmoothHeights(heightMap, heightMapInfo);
        std::vector<VertexStandard> vertices;
        std::vector<unsigned int> inds;
        BuildVertexData(heightMapInfo, heightMap, vertices, 0.2f);
        BuildIndexData(heightMapInfo, inds);
        ComputeNormals(heightMapInfo, heightMap, vertices);
        GLuint VAO, VBO, EBO;
        BuildEBO(vertices, inds, VAO, VBO, EBO);
        terrainVAO = VAO;
        terrainVBO = VBO;
        terrainEBO = EBO;
        indices = inds;
        terrainTexture.InitTexture("Resources/Textures/fakerRomance.png");
        grassTexture.InitTexture("Resources/Textures/grass.png");
        dirtTexture.InitTexture("Resources/Textures/dirt.png");
        stoneTexture.InitTexture("Resources/Textures/rocks.png");
        snowTexture.InitTexture("Resources/Textures/snow.png");
    }
}

void SampleScene::SetupQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

//////////////////////////////////////
// Cleanup                          //
//////////////////////////////////////

SampleScene::~SampleScene() {
    delete mainModel;
    delete skybox;
    if (m_FrameBuffer) {
        delete m_FrameBuffer;
        m_FrameBuffer = nullptr;
    }
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}
