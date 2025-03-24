#include "ShadowScene.h"
#include "ShaderLoader.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include "Skybox.h"
#include "FrameBuffer.h"
#include "ShadowMap.h"
#include "Light.h"
#include "HeightMapInfo.h"
#include <iostream>
#include <gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>

void ShadowScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 0.0f));

    // Load shaders
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");
    Program_color = ShaderLoader::CreateProgram("Resources/Shaders/Color.vert", "Resources/Shaders/Color.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
    shadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMapping.vert", "Resources/Shaders/ShadowMapping.frag");
    instanceShadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMappingInstance.vert", "Resources/Shaders/ShadowMapping.frag");
    mainRenderingShader = ShaderLoader::CreateProgram("Resources/Shaders/MainRendering.vert", "Resources/Shaders/MainRendering.frag");

    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    // Initialize skybox faces
    faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    // Initialize models via game objects
    InitializeModels();
    SetupLights();
    SetupTerrain();

    // Initialize framebuffer and quad for post-processing
    m_FrameBuffer = new FrameBuffer(800, 800);
    SetupQuad();
}

void ShadowScene::InitializeModels() {
    // Create a GameObject for the main model.
    glm::vec3 position(0.0f, 500.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);
    mainModel = new GameObject("MainModel");
    MeshRenderer* mainRenderer = mainModel->AddComponent<MeshRenderer>(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    mainRenderer->SetTexture(ancientTex.GetId());
    mainRenderer->SetShader(Program_Texture);
    AddGameObject(mainModel);

 
}

void ShadowScene::SetupLights() {
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

void ShadowScene::SetupTerrain() {
    HeightMapInfo heightMapInfo = { "Resources/Heightmaps/heightmap.raw", 512, 512, 1.0f };
    std::vector<float> heightMap;
    if (LoadHeightMap(heightMapInfo, heightMap)) {
        SmoothHeights(heightMap, heightMapInfo);

        std::vector<VertexStandard> vertices;
        std::vector<unsigned int> indices;
        BuildVertexData(heightMapInfo, heightMap, vertices, 0.2f);
        BuildIndexData(heightMapInfo, indices);
        ComputeNormals(heightMapInfo, heightMap, vertices);

        GLuint VAO, VBO, EBO;
        BuildEBO(vertices, indices, VAO, VBO, EBO);
        terrainVAO = VAO;
        terrainVBO = VBO;
        terrainEBO = EBO;
        this->indices = indices;

        terrainTexture.InitTexture("Resources/Textures/fakerRomance.png");
        grassTexture.InitTexture("Resources/Textures/grass.png");
        dirtTexture.InitTexture("Resources/Textures/dirt.png");
        stoneTexture.InitTexture("Resources/Textures/rocks.png");
        snowTexture.InitTexture("Resources/Textures/snow.png");
    }
}

void ShadowScene::SetupQuad() {
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

void ShadowScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
/*
    const float radius = 1000.0f;
    const float speed = 1.0f;
    const glm::vec3 center(0.0f, 500.0f, 0.0f);

    float angle = speed * currentFrame;
    float x = center.x + radius * cos(angle);
    float z = center.z + radius * sin(angle);

    // Update main model transform
    mainModel->transform.position = glm::vec3(x, center.y, z);
    float rotationAngle = glm::degrees(atan2(-sin(angle), -cos(angle)));
    mainModel->transform.rotation.y = rotationAngle;
    */
    // Update MeshRenderer component if needed
    MeshRenderer* mainRenderer = mainModel->GetComponent<MeshRenderer>();
    if (mainRenderer) {
        mainRenderer->Update(deltaTime);
    }

    // Update spotlight based on camera
    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;

    // Cycle post-processing effects with TAB key
    static bool tabPressed = false;
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed)
    {
        tabPressed = true;
        currentEffect = static_cast<PostProcessEffect>((static_cast<int>(currentEffect) + 1) % 3);
    }
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        tabPressed = false;
    }
}

void ShadowScene::RenderShadowMap(ShadowMap* shadowMap, const DirectionalLight& dirLight) {
    shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::vec3 lightPos = dirLight.direction * -700.0f;
    shadowMap->UpdateLightSpaceMatrix(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUseProgram(shadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(shadowMap->GetLightSpaceMatrix()));

    // Render the main model's shadow
    glm::mat4 modelMatrix = mainModel->GetWorldMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    MeshRenderer* mainRenderer = mainModel->GetComponent<MeshRenderer>();
    if (mainRenderer) {
        mainRenderer->Render(camera);
    }

 
    shadowMap->Unbind();
}

void ShadowScene::RenderSceneWithShadows() {
    m_FrameBuffer->Bind();
    glViewport(0, 0, 800, 800);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f, mainRenderingShader, "VPMatrix");
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);

    // Render main model
    glUseProgram(Program_Texture);
    MeshRenderer* mainRenderer = mainModel->GetComponent<MeshRenderer>();
    if (mainRenderer) {
        mainRenderer->BindTexture();
        mainRenderer->PassUniforms(camera);
        mainRenderer->PassDirectionalUniforms(dirLight1);
        mainRenderer->PassSpotLightUniforms(spotLight);
    }

    glActiveTexture(GL_TEXTURE2);
    m_ShadowMap1->BindTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(Program_Texture, "shadowMap1"), 2);
    glUniformMatrix4fv(glGetUniformLocation(Program_Texture, "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));

    glActiveTexture(GL_TEXTURE3);
    m_ShadowMap2->BindTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(Program_Texture, "shadowMap2"), 3);
    glUniformMatrix4fv(glGetUniformLocation(Program_Texture, "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));

    if (mainRenderer) {
        mainRenderer->Render(camera);
    }


    // Render terrain
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

    // Post-processing pass
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

void ShadowScene::RenderPostProcessing() {
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

int ShadowScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        Update();
        Render();
        glfwPollEvents();
    }
    return 0;
}

void ShadowScene::Render() {
    RenderShadowMap(m_ShadowMap1, dirLight1);
    RenderShadowMap(m_ShadowMap2, dirLight2);
    RenderSceneWithShadows();
    RenderPostProcessing();
}

ShadowScene::~ShadowScene() {
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
