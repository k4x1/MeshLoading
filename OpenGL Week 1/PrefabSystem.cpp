// PrefabSystem.cpp
#include "PrefabSystem.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ComponentFactory.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

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
