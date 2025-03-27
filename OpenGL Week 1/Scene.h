#pragma once

#include "All.h"   
#include <vector>
#include <string>

class Scene {
public:
    GLFWwindow* Window;
    Camera* camera;
    std::vector<GameObject*> gameObjects;
    std::string sceneName = "scene";

    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera);
    virtual void Start();
    virtual void Update();
    virtual void Render();
    virtual int MainLoop();

    void AddGameObject(GameObject* obj);
    void RemoveGameObject(GameObject* obj);

    void SaveToFile(const std::string& filePath);
    void LoadFromFile(const std::string& filePath);

    virtual ~Scene();
};
