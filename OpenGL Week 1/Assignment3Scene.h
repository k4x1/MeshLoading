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
    ShadowMap* m_ShadowMap1;
    ShadowMap*  m_ShadowMap2;

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
    GLuint instanceShadowMappingShader;
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
    DirectionalLight dirLight1, dirLight2;
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
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}
