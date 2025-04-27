#pragma once
#include <string>
#include "GameObject.h"
#include "EnginePluginAPI.h"

class ENGINE_API PrefabSystem {
public:
    static GameObject* Instantiate(const std::string& prefabPath);
    static bool SavePrefab(GameObject* root, const std::string& filepath);
};
