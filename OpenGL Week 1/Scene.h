#pragma once
#include "All.h"
#include <fstream>
#include <iostream>
class Scene {
public:
    GLFWwindow* Window;
    // Define a camera object
    Camera* camera;
    std::vector<GameObject*> gameObjects;
    std::string sceneName = "scene";
    virtual void InitialSetup(GLFWwindow* _window, Camera* _camera) {}
    virtual void Update() 
    {
 
    }
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

    void SaveToFile(const std::string& filePath)
    {
        json j;
        j["gameObjects"] = json::array();
        for (GameObject* obj : gameObjects)
        {
            j["gameObjects"].push_back(SerializeGameObject(obj));
        }

        std::ofstream outFile(filePath);
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open file for saving: " << filePath << std::endl;
            return;
        }
        outFile << j.dump(4);  
        std::cout << "Saved file to: " << filePath << std::endl;
        outFile.close();
    }

    void LoadFromFile(const std::string& filePath)
    {
        std::ifstream inFile(filePath);
        if (!inFile.is_open())
        {
            std::cerr << "Failed to open scene file: " << filePath << std::endl;
            return;
        }

        json j;
        inFile >> j;
        inFile.close();

        // Clean up current scene objects if necessary
        for (GameObject* obj : gameObjects)
        {
            delete obj;
        }
        gameObjects.clear();

        if (j.contains("gameObjects") && j["gameObjects"].is_array())
        {
            for (const auto& objJson : j["gameObjects"])
            {
                GameObject* obj = DeserializeGameObject(objJson);
                gameObjects.push_back(obj);
            }
        }
        std::cout << "Loaded file to: " << filePath << std::endl;
    }
    virtual ~Scene() { 
        for (auto obj : gameObjects) {
            delete obj;
        }
        gameObjects.clear();
    }
};
