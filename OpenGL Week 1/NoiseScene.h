#pragma once

#include "Scene.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <vector>
#include <string>
#include <fstream>

class Skybox;

class NoiseScene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~NoiseScene();

private:
    GLuint Program_skybox = 0;
    GLuint Program_noise = 0;
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;
    GLFWwindow* Window = nullptr;
    Camera* camera = nullptr;
    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;
    GLuint VAO, VBO, EBO;
    GLuint m_textureID;

    GLuint quadVAO, quadVBO;
    glm::vec3 quadPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    Texture noiseTexture;
    // Function to generate and save Perlin noise texture
    void GenerateAndSaveNoiseTexture();

    // Function to apply gradient coloring to the noise
    glm::vec3 ApplyGradientColor(float value);

    // Function to render the noise texture on a quad
    void RenderNoiseTexture();

    // Function to render animated noise on a second quad
    void RenderAnimatedNoise();

    // Helper function to initialize the quad and shader program
    void InitQuadAndShader();

    // Helper function to initialize the noise texture
    void InitNoiseTexture();
};
