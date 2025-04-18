#pragma once
#include <string>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include "Component.h"

class GameObject;

using CompCreator = std::function<Component* (const nlohmann::json& params, GameObject* owner)>;

class ComponentFactory {
public:
    static ComponentFactory& Instance() {
        static ComponentFactory inst;
        return inst;
    }

    // register a creator under the given name
    void Register(const std::string& typeName, CompCreator c) {
        creators[typeName] = std::move(c);
    }

    // create one (or nullptr if none registered)
    Component* Create(const std::string& typeName,
        const nlohmann::json& params,
        GameObject* owner) const
    {
        auto it = creators.find(typeName);
        if (it == creators.end()) return nullptr;
        return it->second(params, owner);
    }

private:
    std::map<std::string, CompCreator> creators;
};
