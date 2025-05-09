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

class ENGINE_API Scene {
public:

    Scene() = default;
    GLFWwindow* Window = nullptr;
    Camera* camera = nullptr;
    std::vector<GameObject*>   gameObjects;
    std::vector<GameObject*>   goToAdd;
    std::vector<GameObject*>   goToRemove;

    FrameBuffer* gameFrameBuffer = nullptr;

    std::string sceneName = "scene";

    virtual void InitialSetup(GLFWwindow* _window, bool autoLoad = true);
    virtual void Start();
    virtual void Update(float dt);
    virtual void FixedUpdate(float fixedDt);
    virtual void Render(FrameBuffer* currentBuffer, Camera* _camera = nullptr);
    virtual void DrawGizmos(Camera* cam);

    FrameBuffer* GetFrameBuffer();


    nlohmann::json ToJson() const;
    void           FromJson(const nlohmann::json& j);
    std::string    ToString() const;
    void           FromString(const std::string& s);


    void AddGameObject(GameObject* obj);
    void Instantiate(GameObject* obj);
    void RemoveGameObject(GameObject* obj);
    void Destroy(GameObject* obj);

    void SaveToFile(const std::string& filePath);
    void LoadFromFile(const std::string& filePath);

    virtual ~Scene();
};
