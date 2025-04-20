// ComponentFactory.h
#pragma once
#include <string>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include "Component.h"

class GameObject;

// creator: JSON → new Component
using CompCreator = std::function<Component* (const nlohmann::json& params, GameObject* owner)>;
// serializer: Component* → params JSON
using CompSerializer = std::function<nlohmann::json(Component* comp)>;

struct CompEntry {
    CompCreator    create;
    CompSerializer serialize;
};

class ComponentFactory {
public:
    static ComponentFactory& Instance() {
        static ComponentFactory inst;
        return inst;
    }

    // register both creator and serializer under the same typeName
    void Register(const std::string& typeName,
        CompCreator    c,
        CompSerializer s)
    {
        registry[typeName] = { std::move(c), std::move(s) };
    }

    Component* Create(const std::string& typeName,
        const nlohmann::json& params,
        GameObject* owner) const
    {
        auto it = registry.find(typeName);
        if (it == registry.end()) return nullptr;
        return it->second.create(params, owner);
    }

    nlohmann::json Serialize(Component* comp) const {
        for (auto& [typeName, entry] : registry) {
            // we rely on dynamic_cast to check type
            // each serializer should downcast safely
            nlohmann::json j = entry.serialize(comp);
            if (!j.is_null()) {
                // embed the "type" field
                j["type"] = typeName;
                return j;
            }
        }
        return nullptr;
    }

private:
    std::map<std::string, CompEntry> registry;
};
