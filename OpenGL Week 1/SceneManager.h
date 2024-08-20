#pragma once
#include <memory>
#include "Scene.h"

class SceneManager {
public:
    void SwitchScene(int sceneNumber);
    void Update();
    void Render();
    void SetWindow(GLFWwindow* window);

private:
    std::unique_ptr<Scene> currentScene;
    GLFWwindow* window;
};