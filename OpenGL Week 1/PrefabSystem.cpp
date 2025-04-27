// PrefabSystem.cpp
#include "PrefabSystem.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ComponentFactory.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <filesystem>    // C++17
namespace fs = std::filesystem;

GameObject* PrefabSystem::Instantiate(const std::string& prefabPath) {
    // Open and parse the JSON prefab file
    std::ifstream in(prefabPath);
    if (!in.is_open()) {
        std::cerr << "PrefabSystem: Failed to open prefab: " << prefabPath << std::endl;
        return nullptr;
    }
    nlohmann::json j;
    in >> j;
    in.close();

    // Recursive lambda to build GameObject + children
    std::function<GameObject* (const nlohmann::json&)> deserialize =
        [&](const nlohmann::json& objJson) -> GameObject*
        {
            // Create the GameObject
            std::string name = objJson.value("name", "PrefabObject");
            GameObject* go = new GameObject(name);

            // Apply Transform
            if (objJson.contains("transform")) {
                const auto& t = objJson["transform"];
                go->transform.position = {
                    t["position"][0].get<float>(),
                    t["position"][1].get<float>(),
                    t["position"][2].get<float>()
                };
                go->transform.scale = {
                    t["scale"][0].get<float>(),
                    t["scale"][1].get<float>(),
                    t["scale"][2].get<float>()
                };
                auto rot = t["rotation"];
                go->transform.rotation = glm::quat(
                    rot[0].get<float>(),
                    rot[1].get<float>(),
                    rot[2].get<float>(),
                    rot[3].get<float>()
                );
            }

            // Add Components
            if (objJson.contains("components")) {
                for (auto& compJson : objJson["components"]) {
                    std::string type = compJson.value("type", "");
                    Component* c = ComponentFactory::Instance()
                        .Create(type, compJson, go);
                    if (!c)
                        std::cerr << "Unknown component type: " << type << "\n";
                    // ownership is already handled by AddComponent<…>()
                }
            }


            // Recursively deserialize children
            if (objJson.contains("children")) {
                for (auto& childJson : objJson["children"]) {
                    GameObject* child = deserialize(childJson);
                    go->AddChild(child);
                }
            }

            return go;
        };

    // Kick off the recursion from the root JSON
    return deserialize(j);
}


bool PrefabSystem::SavePrefab(GameObject* root, const std::string& filepath) {
    if (!root) {
        std::cout << "[PrefabSystem] SavePrefab called with null root\n";
        return false;
    }

    std::cout << "[PrefabSystem] Saving prefab for GameObject \""
        << root->name << "\" to " << filepath << "\n";

    // Ensure the directory exists:
    fs::path filePathObj(filepath);
    fs::path dir = filePathObj.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        std::cout << "[PrefabSystem] Creating directory: " << dir.string() << "\n";
        std::error_code ec;
        if (!fs::create_directories(dir, ec)) {
            std::cerr << "[PrefabSystem] ERROR: Failed to create directory "
                << dir.string() << ": " << ec.message() << "\n";
            return false;
        }
    }

    // Serialize to JSON
    nlohmann::json j = SerializeGameObject(root);

    // Write file
    std::ofstream out(filepath);
    if (!out.is_open()) {
        std::cerr << "[PrefabSystem] ERROR: Failed to open file for writing: "
            << filepath << "\n";
        return false;
    }
    out << j.dump(4);
    out.close();

    std::cout << "[PrefabSystem] Successfully wrote prefab JSON\n";
    return true;
}
