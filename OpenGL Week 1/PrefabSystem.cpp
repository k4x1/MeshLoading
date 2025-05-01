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
#include <filesystem>  
namespace fs = std::filesystem;

GameObject* PrefabSystem::Instantiate(const std::string& prefabPath) {
    std::ifstream in(prefabPath);
    if (!in.is_open()) {
        std::cerr << "PrefabSystem: Failed to open prefab: " << prefabPath << std::endl;
        return nullptr;
    }
    nlohmann::json j;
    in >> j;
    in.close();

    std::function<GameObject* (const nlohmann::json&)> deserialize =
        [&](const nlohmann::json& objJson) -> GameObject*
        {
            std::string name = objJson.value("name", "PrefabObject");
            GameObject* go = new GameObject(name);

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

            if (objJson.contains("components")) {
                for (auto& compJson : objJson["components"]) {
                    std::string type = compJson.value("type", "");
                    Component* c = ComponentFactory::Instance()
                        .Create(type, compJson, go);
                    if (!c)
                        std::cerr << "Unknown component type: " << type << "\n";
                }
            }


            if (objJson.contains("children")) {
                for (auto& childJson : objJson["children"]) {
                    GameObject* child = deserialize(childJson);
                    go->AddChild(child);
                }
            }

            return go;
        };

    return deserialize(j);
}


bool PrefabSystem::SavePrefab(GameObject* root, const std::string& filepath) {
    if (!root) {
        std::cout << "[PrefabSystem] SavePrefab called with null root\n";
        return false;
    }



    fs::path filePathObj(filepath);
    fs::path dir = filePathObj.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        std::error_code ec;
        if (!fs::create_directories(dir, ec)) {
            std::cerr << "[PrefabSystem] ERROR: Failed to create directory "
                << dir.string() << ": " << ec.message() << "\n";
            return false;
        }
    }

    nlohmann::json j = SerializeGameObject(root);

    std::ofstream out(filepath);
    if (!out.is_open()) {
        std::cerr << "[PrefabSystem] ERROR: Failed to open file for writing: "
            << filepath << "\n";
        return false;
    }
    out << j.dump(4);
    out.close();

    return true;
}
