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
#include <fstream>
#include <iostream>
#include <gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>

void SampleScene::InitialSetup(GLFWwindow* _window)
{
    Scene::InitialSetup(_window);
    Window = _window; // For local use
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);
    // If the camera doesn't have an owner, create one.
    if (camera->owner == nullptr)
    {
        GameObject* camGO = new GameObject("CameraObject");
        camGO->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        camera->owner = camGO;
        AddGameObject(camGO);
    }

    camera->InitCamera(800, 800);

    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");
    Program_color = ShaderLoader::CreateProgram("Resources/Shaders/Color.vert", "Resources/Shaders/Color.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
    shadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMapping.vert", "Resources/Shaders/ShadowMapping.frag");
    instanceShadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMappingInstance.vert", "Resources/Shaders/ShadowMapping.frag");
    mainRenderingShader = ShaderLoader::CreateProgram("Resources/Shaders/MainRendering.vert", "Resources/Shaders/MainRendering.frag");

    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    InitializeModels();
    SetupLights();
    SetupTerrain();
    SetupQuad();

    //gameFrameBuffer = new FrameBuffer(800, 800);
    Start();
}

void SampleScene::Start()
{
    Scene::Start();
    SaveToFile(sceneName + ".json");
}

void SampleScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    for (GameObject* obj : gameObjects) {
        obj->Update(static_cast<float>(deltaTime));
    }

    // Update the spotlight based on the camera's owner's transform:
    spotLight.position = camera->owner->transform.position;
    {
        // Compute the camera's forward direction using its owner's rotation.
        float pitch = camera->owner->transform.rotation.x;
        float yaw = camera->owner->transform.rotation.y;
        glm::vec3 direction;
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        spotLight.direction = glm::normalize(direction);
    }

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
    Scene::Update();
}

void SampleScene::Render(FrameBuffer* currentBuffer, Camera* _camera) {
    RenderShadowMap(m_ShadowMap1, dirLight1);
    RenderShadowMap(m_ShadowMap2, dirLight2);
    
    currentBuffer->Bind();
    glViewport(0, 0, 800, 800);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update camera matrices
    glm::mat4 view;
    glm::mat4 projection;
    Camera* selectedCamera;
    if(_camera == nullptr)
    { 
        camera->Matrix(0.01f, 1000.0f, mainRenderingShader, "VPMatrix");
        view = camera->m_view;
        projection = camera->m_projection;
        selectedCamera = camera;
    }
    else
    {
        _camera->Matrix(0.01f, 1000.0f, mainRenderingShader, "VPMatrix");
        view = _camera->m_view;
        projection = _camera->m_projection;
        selectedCamera = _camera;
    }
    skybox->Render(view, projection);
    
    RenderSceneWithShadows(selectedCamera);
    currentBuffer->Unbind();
  /*  sceneFrameBuffer->Bind();
    glViewport(0, 0, 800, 600);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
    //// Update the scene camera based on input.
    //sceneCamera->Update(deltaTime); // Ensure this is updated with current mouse/scroll input.
    //glm::mat4 sceneView = sceneCamera->GetViewMatrix();
    //glm::mat4 sceneProjection = sceneCamera->GetProjectionMatrix();
    //SetShaderMatrices(sceneView, sceneProjection);
    //RenderSceneWithShadows();
    //sceneFrameBuffer->Unbind();
   // RenderPostProcessing();
    Scene::Render(currentBuffer);
}

void SampleScene::InitializeModels() {
    glm::vec3 position(0.0f, 500.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);
    mainModel = new GameObject("MainModel");
    MeshRenderer* mainRenderer = mainModel->AddComponent<MeshRenderer>(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    mainRenderer->SetTexture(ancientTex.GetId());
    mainRenderer->SetShader(Program_Texture);
    AddGameObject(mainModel);
}

void SampleScene::SetupLights() {
    m_ShadowMap1 = new ShadowMap();
    m_ShadowMap2 = new ShadowMap();

    dirLight1.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight1.color = glm::vec3(1.0f, 1.0f, 1.0f);

    dirLight2.direction = glm::vec3(0.3f, -1.0f, 0.2f);
    dirLight2.color = glm::vec3(0.8f, 0.8f, 0.8f);

    // Set spotlight using camera's owner's transform and computed forward direction.
    spotLight.position = camera->owner->transform.position;
    {
        float pitch = camera->owner->transform.rotation.x;
        float yaw = camera->owner->transform.rotation.y;
        glm::vec3 direction;
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        spotLight.direction = glm::normalize(direction);
    }
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

void SampleScene::RenderShadowMap(ShadowMap* shadowMap, const DirectionalLight& dirLight) {
    shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::vec3 lightPos = dirLight.direction * -700.0f;
    shadowMap->UpdateLightSpaceMatrix(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUseProgram(shadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(shadowMap->GetLightSpaceMatrix()));

    // Render the main model's shadow.
    glm::mat4 modelMatrix = mainModel->GetWorldMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    MeshRenderer* mainRenderer = mainModel->GetComponent<MeshRenderer>();
    if (mainRenderer) {
        mainRenderer->Render(camera);
    }
    shadowMap->Unbind();
}

void SampleScene::RenderSceneWithShadows(Camera* _camera) {
   

    glUseProgram(Program_Texture);
    MeshRenderer* mainRenderer = mainModel->GetComponent<MeshRenderer>();
    if (mainRenderer) {
        mainRenderer->BindTexture();
        mainRenderer->PassUniforms(_camera);
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

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_Texture, "skybox"), 1);

    if (mainRenderer) {
        mainRenderer->Render(_camera);
    }

    glUseProgram(Program_terrain);
    glBindVertexArray(terrainVAO);
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "view"), 1, GL_FALSE, glm::value_ptr(_camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos1"), 1, glm::value_ptr(dirLight1.direction * -700.0f));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos2"), 1, glm::value_ptr(dirLight2.direction * -700.0f));
    // Replace camera->m_position with camera->owner->transform.position if needed:
    glUniform3fv(glGetUniformLocation(Program_terrain, "viewPos"), 1, glm::value_ptr(_camera->owner->transform.position));
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


    glViewport(0, 0, 800, 800);
    glUseProgram(postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), static_cast<int>(currentEffect));
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
 /*   gameFrameBuffer->BindTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);*/
 
}

void SampleScene::RenderPostProcessing() {
    glViewport(0, 0, 800, 800);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), static_cast<int>(currentEffect));
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    gameFrameBuffer->BindTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

SampleScene::~SampleScene() {
    delete mainModel;
    delete skybox;
    if (gameFrameBuffer) {
        delete gameFrameBuffer;
        gameFrameBuffer = nullptr;
    }
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}
