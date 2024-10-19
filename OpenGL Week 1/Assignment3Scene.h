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
class Assignment3Scene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~Assignment3Scene();

private:
    FrameBuffer* m_FrameBuffer = nullptr;
    ShadowMap* m_ShadowMap;

    MeshModel* model = nullptr;
    MeshModel* pointLight1 = nullptr;
    MeshModel* pointLight2 = nullptr;
    InstanceMeshModel* instanceModel = nullptr;

    GLuint quadVAO, quadVBO;
    GLuint postProcessingShader;


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
    GLuint mainRenderingShader;

    Texture ancientTex;
    Texture scifiTex;
    Texture skyboxTex;
    Texture blankTex;

    float AmbientStrength;
    glm::vec3 AmbientColor;
    static const int MAX_POINT_LIGHTS = 4;
    unsigned int PointLightCount;

    PointLight PointLightArray[MAX_POINT_LIGHTS];
    DirectionalLight dirLight;
    SpotLight spotLight;

    // HeightMap-related members
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
};
