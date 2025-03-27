#include "SceneManager.h"
#include "SampleScene.h"

// Include other scenes

void SceneManager::SwitchScene(int sceneNumber) {
    switch (sceneNumber) {
    case 1:
        currentScene = std::make_unique<SampleScene>();
        break;

    default:
        return;
    }
    currentScene->InitialSetup(this->window, this->camera);
}

void SceneManager::Update() {
    if (currentScene) {
        currentScene->Update();
    }
}

void SceneManager::Render() {
    if (currentScene) {
        currentScene->Render();
    }
}

void SceneManager::SetWindow(GLFWwindow* window) {
    this->window = window;
}