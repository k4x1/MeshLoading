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

class NoiseScene : public Scene {
public:
    void InitialSetup(GLFWwindow* _window, Camera* _camera) override;
    void Update() override;
    void Render() override;
    int MainLoop() override;
    ~NoiseScene();

private:
    GLuint Program_skybox = 0;
    Skybox* skybox = nullptr;
    std::vector<std::string> faces;
    GLFWwindow* Window = nullptr;
    Camera* camera = nullptr;
    double currentFrame = 0;
    double lastFrame = 0;
    double deltaTime = 0;
};
