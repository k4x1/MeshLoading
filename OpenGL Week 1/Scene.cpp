#include "Scene.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "PhysicsEngine.h"

void Scene::InitialSetup(GLFWwindow* _window, bool autoLoad) {
    Window = _window;
    camera= new Camera();
}

void Scene::Start()
{
    for (GameObject* obj : gameObjects) {
        obj->Start();
    }
}

void Scene::Update(float dt) {
    for (GameObject* obj : gameObjects) {

        obj->Update(dt);
    }
}
void Scene::FixedUpdate(float fixedDt) {
    Physics::PhysicsEngine::Instance().Update(fixedDt);

    for (auto* go : gameObjects)
        go->FixedUpdate(fixedDt);
}
void Scene::Render(FrameBuffer* currentBuffer, Camera* _camera) {
    Camera* renderCamera = (_camera != nullptr) ? _camera : camera;
    for (GameObject* obj : gameObjects) {
        obj->Render(camera);
    }
}

FrameBuffer* Scene::GetFrameBuffer()
{
    return gameFrameBuffer;
}

void Scene::AddGameObject(GameObject* obj) {
    gameObjects.push_back(obj);
}

void Scene::RemoveGameObject(GameObject* obj) {
    if (obj->parent) {
        auto& sib = obj->parent->children;
        sib.erase(std::remove(sib.begin(), sib.end(), obj), sib.end());
    }
    gameObjects.erase(
        std::remove(gameObjects.begin(), gameObjects.end(), obj),
        gameObjects.end()
    );
    delete obj;
}


nlohmann::json Scene::ToJson() const {
    nlohmann::json j;
    j["gameObjects"] = nlohmann::json::array();
    for (auto* go : gameObjects)
        j["gameObjects"].push_back(SerializeGameObject(go));
    return j;
}

void Scene::FromJson(const nlohmann::json& j) {
    for (auto* go : gameObjects)
    {
        delete go;
        gameObjects.clear();
    }

    for (auto& objJ : j["gameObjects"])
        gameObjects.push_back(DeserializeGameObject(objJ));
}

std::string Scene::ToString() const {
    return ToJson().dump();
}

void Scene::FromString(const std::string& s) {
    FromJson(nlohmann::json::parse(s));
}
void Scene::SaveToFile(const std::string& fileName) {
    namespace fs = std::filesystem;
    fs::path outPath = fs::path("Assets") / fileName;

    nlohmann::json j;
    j["gameObjects"] = nlohmann::json::array();
    for (GameObject* obj : gameObjects) {
        j["gameObjects"].push_back(SerializeGameObject(obj));
    }

    std::ofstream outFile(outPath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for saving: " << outPath << std::endl;
        return;
    }
    outFile << j.dump(4);
    std::cout << "Saved scene to: " << outPath << std::endl;
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
    if (j.contains("gameObjects")) {
        for (auto& objJson : j["gameObjects"]) {
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
