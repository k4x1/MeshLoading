#include "Scene.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>


void Scene::InitialSetup(GLFWwindow* _window, Camera* _camera) {
    Window = _window;
    camera = _camera;
}

void Scene::Start()
{
    for (GameObject* obj : gameObjects) {
        obj->Start();
    }
}

void Scene::Update() {
    float dt = 0.016f;
    for (GameObject* obj : gameObjects) {

        obj->Update(dt);
    }
}

void Scene::Render() {
    for (GameObject* obj : gameObjects) {
        obj->Render(camera);
    }
}

int Scene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        Update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();

        glfwSwapBuffers(Window);
    }
    return 0;
}

void Scene::AddGameObject(GameObject* obj) {
    gameObjects.push_back(obj);
}

void Scene::RemoveGameObject(GameObject* obj) {
    auto it = std::find(gameObjects.begin(), gameObjects.end(), obj);
    if (it != gameObjects.end()) {
        gameObjects.erase(it);
    }
}

void Scene::SaveToFile(const std::string& filePath) {
    nlohmann::json j;
    j["gameObjects"] = nlohmann::json::array();
    for (GameObject* obj : gameObjects) {
        j["gameObjects"].push_back(SerializeGameObject(obj));
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for saving: " << filePath << std::endl;
        return;
    }
    outFile << j.dump(4);
    std::cout << "Saved file to: " << filePath << std::endl;
    outFile.close();
}

void Scene::LoadFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open scene file: " << filePath << std::endl;
        return;
    }
    nlohmann::json j;
    inFile >> j;
    inFile.close();

 
    for (GameObject* obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJson : j["gameObjects"]) {
            GameObject* obj = DeserializeGameObject(objJson);
            gameObjects.push_back(obj);
        }
    }
    std::cout << "Loaded file from: " << filePath << std::endl;
}

Scene::~Scene() {
    for (GameObject* obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}
