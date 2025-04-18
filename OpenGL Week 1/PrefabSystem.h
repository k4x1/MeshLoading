#pragma once
#include <string>
#include "GameObject.h"

class PrefabSystem {
public:
    static GameObject* Instantiate(const std::string& prefabPath);
};
