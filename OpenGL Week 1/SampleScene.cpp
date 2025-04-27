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
#include "TerrainComponent.h"
#include "CameraMovement.h"
#include <iostream>
#include <fstream>
#include <gtc/type_ptr.hpp>
//using fs = 

void SampleScene::InitialSetup(GLFWwindow* window, bool autoLoad) {
    // base init
    Scene::InitialSetup(window);
    Window = window;

    // GL state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, 800, 800);

    // 1) Optional persistent load
    if (autoLoad) {
        std::string path = "Assets/" + sceneName + ".json";
        if (fs::exists(path)) {
            LoadFromFile(path);
        }
    }

    // 2) Find first Camera component in loaded scene
    for (auto* go : gameObjects) {
        if (go->GetComponent<Camera>()) {
            camera->owner = go;
            break;
        }
    }

    // 3) Fallback: create a default camera if none found
    if (!camera->owner) {
        GameObject* camGO = new GameObject("CameraObject");
        camGO->AddComponent<Camera>();
        AddGameObject(camGO);
        camera->owner = camGO;
    }

    // 4) Initialize camera’s projection parameters
    camera->InitCamera(800, 800);

    // — load shaders/textures/skybox —
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
    shadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMapping.vert", "Resources/Shaders/ShadowMapping.frag");
    mainRenderingShader = ShaderLoader::CreateProgram("Resources/Shaders/MainRendering.vert", "Resources/Shaders/MainRendering.frag");

    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    std::vector<std::string> faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    // frame-buffer for post-processing pass
    gameFrameBuffer = new FrameBuffer(800, 800);
    gameFrameBuffer->Initialize();

    // Setup helpers
    SetupLights();
    SetupQuad();

    // Finally fire any Start() logic
    Start();
}


void SampleScene::Start()
{
    Scene::Start();

}

void SampleScene::Update(float dt)
{
    double current = glfwGetTime();
    deltaTime = static_cast<float>(current - lastFrame);
    lastFrame = current;

    for (auto* obj : gameObjects)
        obj->Update(deltaTime);

    // Toggle post-process effect
    static bool tabPressed = false;
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        currentEffect = static_cast<PostProcessEffect>((static_cast<int>(currentEffect) + 1) % 3);
    }
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_RELEASE)
        tabPressed = false;

    Scene::Update(dt);
}
void SampleScene::Render(FrameBuffer* currentBuffer, Camera* _camera)
{
    // 1) Shadow pass
    RenderShadowMap(m_ShadowMap1, dirLight1);
    RenderShadowMap(m_ShadowMap2, dirLight2);

    // 2) Main scene into FBO
    currentBuffer->Bind();
    glViewport(0, 0, 800, 800);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera* cam = _camera ? _camera : camera;
    cam->Matrix(0.01f, 1000.0f, Program_Texture, "VPMatrix");

    // 2a) Skybox
    skybox->Render(cam->GetViewMatrix(), cam->GetProjectionMatrix());

    // 2b) Lit textured pass
    glUseProgram(Program_Texture);

    // Ambient
    glUniform1f(glGetUniformLocation(Program_Texture, "AmbientStrength"), 1.0f);
    glUniform3f(glGetUniformLocation(Program_Texture, "AmbientColor"), 1.0f, 1.0f, 1.0f);

    // Camera position
    glm::vec3 camPos = camera->owner->transform.position;
    glUniform3fv(glGetUniformLocation(Program_Texture, "CameraPos"), 1, glm::value_ptr(camPos));

    // Directional light
    glUniform3fv(glGetUniformLocation(Program_Texture, "DirLight.direction"), 1, glm::value_ptr(dirLight1.direction));
    glUniform3fv(glGetUniformLocation(Program_Texture, "DirLight.color"), 1, glm::value_ptr(dirLight1.color));
    glUniform1f(glGetUniformLocation(Program_Texture, "DirLight.specularStrength"), dirLight1.specularStrength);

    // Spotlight
    glUniform3fv(glGetUniformLocation(Program_Texture, "SptLight.position"), 1, glm::value_ptr(spotLight.position));
    glUniform3fv(glGetUniformLocation(Program_Texture, "SptLight.direction"), 1, glm::value_ptr(spotLight.direction));
    glUniform3fv(glGetUniformLocation(Program_Texture, "SptLight.color"), 1, glm::value_ptr(spotLight.color));
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.specularStrength"), spotLight.specularStrength);
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.attenuationConstant"), spotLight.attenuationConstant);
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.attenuationLinear"), spotLight.attenuationLinear);
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.attenuationExponent"), spotLight.attenuationExponent);
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.innerCutoff"), spotLight.innerCutoff);
    glUniform1f(glGetUniformLocation(Program_Texture, "SptLight.outerCutoff"), spotLight.outerCutoff);

    // Bind reflection cubemap to unit 5
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_Texture, "skybox"), 5);

    // Draw every GameObject (MeshRenderer will bind its Texture0)
    for (auto* obj : gameObjects)
        obj->Render(cam);

    glUseProgram(0);
    currentBuffer->Unbind();

    // 3) Post‑processing
    glViewport(0, 0, 800, 800);
    glUseProgram(postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), int(currentEffect));

    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);

    gameFrameBuffer->BindTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glUseProgram(0);
}


void SampleScene::SetupLights() {
    constexpr unsigned int SHADOW_W = 2048;
    constexpr unsigned int SHADOW_H = 2048;

    m_ShadowMap1 = new ShadowMap(SHADOW_W, SHADOW_H);
    m_ShadowMap2 = new ShadowMap(SHADOW_W, SHADOW_H);


    glDisable(GL_CULL_FACE);
}


void SampleScene::SetupQuad()
{
    float quadVertices[] = {
        // positions  // texCoords
        -1,  1, 0,1,
        -1, -1, 0,0,
         1, -1, 1,0,
        -1,  1, 0,1,
         1, -1, 1,0,
         1,  1, 1,1
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),  quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SampleScene::RenderShadowMap(ShadowMap* shadowMap, const DirectionalLight& dirLight)
{
    shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    // Update light-space matrix & draw scene depth using mainModel only or entire scene as needed
    shadowMap->Unbind();
}

SampleScene::~SampleScene()
{
    delete skybox;
    if (gameFrameBuffer) delete gameFrameBuffer;
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}
