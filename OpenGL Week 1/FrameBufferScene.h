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
class FrameBufferScene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~FrameBufferScene();

private:
    // Define model pointers
    MeshModel* model = nullptr;
    MeshModel* pointLight1 = nullptr;
    MeshModel* pointLight2 = nullptr;
    InstanceMeshModel* instanceModel = nullptr;

    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rbo;

    // Quad for rendering the framebuffer texture
    GLuint quadVAO, quadVBO;

    // Shader for post-processing
    GLuint postProcessingShader;

    // Enum for post-processing effects
    enum class PostProcessEffect {
        None,
        Inversion,
        Greyscale
    };

    PostProcessEffect currentEffect = PostProcessEffect::Greyscale;
      // Define program IDs for shaders

    GLuint Program_Texture = 0;
    GLuint Program_instanceTexture = 0;
    GLuint Program_color = 0;

    // Define variables for movement
    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime;
    float modelSpeed = 100.0f;

    // Define textures
    Texture ancientTex;
    Texture scifiTex;
    Texture skyboxTex;
    Texture blankTex;

    // Define a Skybox object
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;

    float AmbientStrength;
    glm::vec3 AmbientColor;
    static const int MAX_POINT_LIGHTS = 4;
    unsigned int PointLightCount;

    PointLight PointLightArray[MAX_POINT_LIGHTS];
    DirectionalLight dirLight;
    SpotLight spotLight;
    GLFWwindow* Window = nullptr;
};
