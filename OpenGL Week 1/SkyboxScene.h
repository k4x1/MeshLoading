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
#include "Skybox.h"
#include "InputManager.h"

class SkyboxScene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~SkyboxScene();

private:
    // Define a camera object

    GLuint Program_Texture = 0;
    // Define program ID for skybox shader
    GLuint Program_skybox = 0;
    // Load terrain shader
    GLuint Program_terrain = 0;

    // Define a Skybox object
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;

    GLuint terrainVAO, terrainVBO, terrainEBO;
    GLuint terrainTexture;
    std::vector<unsigned int> indices;
    // Define input manager
    InputManager* inputs = nullptr;

    // Define variables for movement
    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;

    GLFWwindow* Window = nullptr;
};
