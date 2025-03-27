#pragma once
#include "Scene.h"
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "ShaderLoader.h"
#include "Camera.h"
#include "Texture.h"
#include "InputManager.h"
#include "Light.h"
#include "Skybox.h"
#include "FrameBuffer.h"
#include "ShadowMap.h"

class SampleScene : public Scene {
public:
    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual int MainLoop() override;
    virtual ~SampleScene();

private:
    // Extra passes
    FrameBuffer* m_FrameBuffer = nullptr;
    ShadowMap* m_ShadowMap1 = nullptr;
    ShadowMap* m_ShadowMap2 = nullptr;

    // Game objects are added via the base Scene class.
    // For example, mainModel is created and gets a MeshRenderer.
    GameObject* mainModel = nullptr;

    // Post-processing quad data.
    GLuint quadVAO = 0, quadVBO = 0;
    GLuint postProcessingShader = 0;
    enum class PostProcessEffect {
        None,
        Inversion,
        Greyscale
    };
    PostProcessEffect currentEffect = PostProcessEffect::None;

    // Shader programs for extra passes.
    GLuint Program_Texture = 0;
    GLuint Program_instanceTexture = 0;
    GLuint Program_color = 0;
    GLuint shadowMappingShader = 0;
    GLuint instanceShadowMappingShader = 0;
    GLuint mainRenderingShader = 0;

    // Textures.
    Texture ancientTex;
    Texture scifiTex;
    Texture skyboxTex;
    Texture blankTex;

    // Lights.
    DirectionalLight dirLight1;
    DirectionalLight dirLight2;
    SpotLight spotLight;

    // Terrain members.
    GLuint Program_terrain = 0;
    GLuint terrainVAO = 0, terrainVBO = 0, terrainEBO = 0;
    Texture terrainTexture;
    Texture grassTexture, dirtTexture, stoneTexture, snowTexture;
    std::vector<unsigned int> indices;

    // Skybox.
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;
    GLuint Program_skybox = 0;

    // Timing.
    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;

    // Local window pointer.
    GLFWwindow* Window = nullptr;

    // Helper functions.
    void InitializeModels();
    void SetupLights();
    void SetupTerrain();
    void SetupQuad();

    // Extra rendering passes.
    void RenderShadowMap();
    void RenderSceneExtras();
    void RenderPostProcessing();
};
