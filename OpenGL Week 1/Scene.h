#pragma once
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
#include "InputManager.h"
#include "Light.h"
#include <string> 
#include "Skybox.h"
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include <imgui.h>  
#include "FrameBuffer.h"

class Scene {
public:
    GLFWwindow* Window;
    Camera* camera;
    std::vector<GameObject*> gameObjects;
    FrameBuffer* m_FrameBuffer = nullptr;

    std::string sceneName = "scene";

    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera);
    virtual void Start();
    virtual void Update();
    virtual void Render();
    virtual int MainLoop();
    FrameBuffer* GetFrameBuffer();

    void AddGameObject(GameObject* obj);
    void RemoveGameObject(GameObject* obj);

    void SaveToFile(const std::string& filePath);
    void LoadFromFile(const std::string& filePath);

    virtual ~Scene();
};
