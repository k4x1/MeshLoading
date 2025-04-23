#pragma once
#include <string>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include "Component.h"
#include <iostream>

class GameObject;
using CompCreator = std::function<Component* (const nlohmann::json& params, GameObject* owner)>;
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
        std::cout << "[ComponentFactory] Create() entry for type=\""
            << typeName << "\" owner=" << owner << std::endl;

        auto it = registry.find(typeName);
        if (it == registry.end()) {
            std::cout << "[ComponentFactory]  → NO SUCH TYPE\n";
            return nullptr;
        }

        Component* comp = nullptr;
        try {
            comp = it->second.create(params, owner);
            std::cout << "[ComponentFactory]  → created "
                << comp << " (owner=" << comp->owner << ")\n";
            
        }
        catch (const std::exception& ex) {
            std::cout << "[ComponentFactory]  **EXCEPTION** " << ex.what() << std::endl;
        }
        return comp;
    }


    // expose the map so UI can iterate over it
    const std::map<std::string, CompEntry>& GetRegistry() const {
        return registry;
    }

    nlohmann::json Serialize(Component* comp) const {
        for (auto& [typeName, entry] : registry) {
            nlohmann::json j = entry.serialize(comp);
            if (!j.is_null()) {
                j["type"] = typeName;
                return j;
            }
        }
        return nullptr;
    }

private:
    std::map<std::string, CompEntry> registry;
};
