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
#include "GameObject.h"
#include "MeshRenderer.h"
class Scene {
public:
    GLFWwindow* Window;
    // Define a camera object
    Camera* camera;
    std::vector<GameObject*> gameObjects;
    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera) {}
    virtual void Update() {}
    virtual void Render() {}
    virtual int MainLoop() { return 0; }
    void AddGameObject(GameObject* obj) {
        gameObjects.push_back(obj);
    }

    void RemoveGameObject(GameObject* obj) {
        auto it = std::find(gameObjects.begin(), gameObjects.end(), obj);
        if (it != gameObjects.end()) {
            gameObjects.erase(it);
        }
    }

    virtual ~Scene() { 
        for (auto obj : gameObjects) {
            delete obj;
        }
        gameObjects.clear();
    }
};
