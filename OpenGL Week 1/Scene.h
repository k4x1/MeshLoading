#pragma once
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
class Scene {
public:
    GLFWwindow* Window;
    // Define a camera object
    Camera* camera;
    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera) {}
    virtual void Update() {}
    virtual void Render() {}
    virtual int MainLoop() { return 0; }
    virtual ~Scene() = default;
};