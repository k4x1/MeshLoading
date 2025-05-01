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

static std::unordered_map<std::string, nlohmann::json> jsonCache;
static std::unordered_map<std::string, GameObject*>   protoCache;

GameObject* PrefabSystem::Instantiate(const std::string& prefabPath) {
    auto it = jsonCache.find(prefabPath);
    if (it == jsonCache.end()) {
        std::ifstream in(prefabPath);
        if (!in) {
            DEBUG_ERR("PrefabSystem: Failed to open prefab: " << prefabPath);
            return nullptr;
        }
        nlohmann::json j; in >> j;
        jsonCache[prefabPath] = j;
        it = jsonCache.find(prefabPath);
    }

    return cloneFromJson(it->second);
}

GameObject* PrefabSystem::cloneFromJson(const nlohmann::json& objJson) {
    std::string name = objJson.value("name", "PrefabObject");
    GameObject* go = new GameObject(name);

    // transform
    if (auto t = objJson.find("transform"); t != objJson.end()) {
        const auto& tt = *t;
        go->transform.position = {
            tt["position"][0], tt["position"][1], tt["position"][2]
        };
        go->transform.scale = {
            tt["scale"][0], tt["scale"][1], tt["scale"][2]
        };
        auto& rot = tt["rotation"];
        go->transform.rotation = glm::quat(rot[0], rot[1], rot[2], rot[3]);
    }

    // components
    if (auto comps = objJson.find("components"); comps != objJson.end()) {
        for (auto& cj : *comps) {
            std::string type = cj.value("type", "");
            Component* c = ComponentFactory::Instance().Create(type, cj, go);
            if (!c) DEBUG_ERR("Unknown component type: " << type);
        }
    }

    // children
    if (auto kids = objJson.find("children"); kids != objJson.end()) {
        for (auto& cj : *kids) {
            GameObject* child = cloneFromJson(cj);
            go->AddChild(child);
        }
    }

    return go;
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
