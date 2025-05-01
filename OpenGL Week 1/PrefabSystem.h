#pragma once
#include <string>
#include <unordered_map>
#include "GameObject.h"
#include "EnginePluginAPI.h"

class ENGINE_API PrefabSystem {
public:
    static GameObject* Instantiate(const std::string& prefabPath);
    static bool SavePrefab(GameObject* root, const std::string& filepath);
private: 

    static GameObject* cloneFromJson(const nlohmann::json& j);
  //  static GameObject* cloneFromPrototype(GameObject* proto);
};
