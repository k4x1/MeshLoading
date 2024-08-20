#include "SceneManager.h"
#include "GameScene.h"

// Include other scenes

void SceneManager::SwitchScene(int sceneNumber) {
    switch (sceneNumber) {
    case 1:
        currentScene = std::make_unique<GameScene>();
        break;
    case 2:
      //  currentScene = std::make_unique<Scene2>();
        break;
        // Add cases for other scenes
    default:
        return;
    }
    currentScene->InitialSetup();
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