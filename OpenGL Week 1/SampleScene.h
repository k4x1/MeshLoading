#pragma once
#include "Scene.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <string>
#include "ShaderLoader.h"
#include "Camera.h"
#include "Texture.h"
#include "InputManager.h"
#include "Light.h"
#include "Skybox.h"
#include "ShadowMap.h"

class ENGINE_API SampleScene : public Scene {
public:
    SampleScene() = default;
    void InitialSetup(GLFWwindow* _window, bool autoLoad = true) override;
    void Start() override;
    void Update(float dts) override;
    void Render(FrameBuffer* currentBuffer, Camera* _camera = nullptr) override;
    virtual ~SampleScene();

private:
    ShadowMap* m_ShadowMap1 = nullptr; 
    ShadowMap* m_ShadowMap2 = nullptr;

    MeshModel* pointLight1 = nullptr;
    MeshModel* pointLight2 = nullptr;
    GameObject* mainModel = nullptr;

    GLuint quadVAO = 0, quadVBO = 0;
    GLuint postProcessingShader = 0;

    enum class PostProcessEffect {
        None,
        Inversion,
        Greyscale
    };
    PostProcessEffect currentEffect = PostProcessEffect::None;

    GLuint Program_Texture = 0;
    GLuint Program_instanceTexture = 0;
    GLuint Program_color = 0;
    GLuint shadowMappingShader = 0;
    GLuint instanceShadowMappingShader = 0;
    GLuint mainRenderingShader = 0;

    Texture ancientTex;
    Texture scifiTex;
    Texture skyboxTex;
    Texture blankTex;

    float AmbientStrength;
    glm::vec3 AmbientColor;
    DirectionalLight dirLight1;
    DirectionalLight dirLight2;
    SpotLight spotLight;

    GLuint Program_terrain = 0;
    GLuint terrainVAO = 0, terrainVBO = 0, terrainEBO = 0;
    Texture terrainTexture;
    Texture grassTexture, dirtTexture, stoneTexture, snowTexture;
    std::vector<unsigned int> indices;

    Skybox* skybox = nullptr;
    std::vector<std::string> faces;
    GLuint Program_skybox = 0;

    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;
    float modelSpeed = 100.0f;

    GLFWwindow* Window = nullptr;

    void SetupLights();
    void SetupQuad();

    void RenderShadowMap(ShadowMap* shadowMap, const DirectionalLight& dirLight);
};
