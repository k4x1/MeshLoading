#pragma once
#include "Scene.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderLoader.h"
#include "Camera.h"
#include "Texture.h"
#include "InstanceMeshModel.h"
#include "InputManager.h"
#include "Light.h"
#include <string>
#include "Skybox.h"
#include "FrameBuffer.h"
#include "ShadowMap.h"

class DeferredRenderingScene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~DeferredRenderingScene();

private:
    FrameBuffer* m_FrameBuffer = nullptr;
    ShadowMap* m_ShadowMap1 = nullptr; // First shadow map
    ShadowMap* m_ShadowMap2 = nullptr; // Second shadow map for the additional light

    MeshModel* model = nullptr;
    std::vector<MeshModel*> models;
    std::vector<MeshModel*> lightSourceModels;

    InstanceMeshModel* instanceModel;

    GLuint quadVAO, quadVBO;
    GLuint postProcessingShader;
    GLuint geometryShader;
    GLuint lightingShader;
    GLuint lightSourceShader;

    enum class PostProcessEffect {
        None,
        Inversion,
        Greyscale
    };

    PostProcessEffect currentEffect = PostProcessEffect::None;

    GLuint Program_Texture = 0;
    GLuint Program_instanceTexture = 0;
    GLuint Program_color = 0;
    GLuint shadowMappingShader;
    GLuint instanceShadowMappingShader;
    GLuint mainRenderingShader;

    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    Texture ancientTex;
    Texture scifiTex;
    Texture skyboxTex;
    Texture blankTex;

    float AmbientStrength;
    glm::vec3 AmbientColor;
    static const int MAX_POINT_LIGHTS = 10;
    unsigned int PointLightCount;

    std::vector<PointLight> pointLights;
    DirectionalLight dirLight1;
    DirectionalLight dirLight2;
    SpotLight spotLight;


    GLuint Program_terrain = 0;
    GLuint terrainVAO, terrainVBO, terrainEBO;
    Texture terrainTexture;
    Texture grassTexture, dirtTexture, stoneTexture, snowTexture;
    std::vector<unsigned int> indices;

    // Common members
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;
    GLuint Program_skybox = 0;

    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;
    float modelSpeed = 100.0f;

    GLFWwindow* Window = nullptr;

    void SetupGBuffer();
    void GeometryPass();
    void LightingPass();
    void RenderLightSources();
};
