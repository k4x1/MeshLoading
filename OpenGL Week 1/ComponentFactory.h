#pragma once
#include <string>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include "Component.h"
#include <iostream>
#include "GameObject.h"      
#include "EnginePluginAPI.h"

using CompCreator = std::function<Component* (const nlohmann::json& params, GameObject* owner)>;
using CompSerializer = std::function<nlohmann::json(Component* comp)>;

struct ENGINE_API CompEntry {
    CompCreator    create;
    CompSerializer serialize;
};

class ENGINE_API ComponentFactory {
public:
    static ComponentFactory& Instance() {
        static ComponentFactory inst;
        return inst;
    }

    void Register(const std::string& typeName,
        CompCreator    c,
        CompSerializer s)
    {
        registry_[typeName] = { std::move(c), std::move(s) };
    }

    Component* Create(const std::string& typeName,
        const nlohmann::json& params,
        GameObject* owner) const
    {
        std::cout << "[ComponentFactory] Create() entry for type=\""
            << typeName << "\" owner=" << owner << std::endl;

        auto it = registry_.find(typeName);
        if (it == registry_.end()) {
            std::cout << "[ComponentFactory] NO SUCH TYPE\n";
            return nullptr;
        }

        try {
            Component* comp = it->second.create(params, owner);
            std::cout << "[ComponentFactory] created "
                << comp << " (owner=" << comp->owner << ")\n";
            return comp;
        }
        catch (const std::exception& ex) {
            std::cout << "[ComponentFactory]  **EXCEPTION** " << ex.what() << std::endl;
            return nullptr;
        }
    }

    const std::map<std::string, CompEntry>& GetRegistry() const {
        return registry_;
    }

    nlohmann::json Serialize(Component* comp) const {
        for (auto& [typeName, entry] : registry_) {
            nlohmann::json j = entry.serialize(comp);
            if (!j.is_null()) {
                j["type"] = typeName;
                return j;
            }
        }
        return nullptr;
    }

private:
    std::map<std::string, CompEntry> registry_;
};
#define REGISTER_SERIALIZABLE_COMPONENT(TYPE)                                       \
namespace {                                                                         \
  static bool _##TYPE##_registered = []{                                            \
    ComponentFactory::Instance().Register(                                          \
      #TYPE,                                                                        \
      [](const nlohmann::json& j, GameObject* owner)->Component* {                  \
        TYPE* c = owner->AddComponent<TYPE>();                                      \
        c->Deserialize(j);                                                          \
        return c;                                                                   \
      },                                                                            \
      [](Component* base)->nlohmann::json {                                         \
        if (TYPE* p = dynamic_cast<TYPE*>(base))                                    \
          return p->Serialize();                                                    \
        return nullptr;                                                             \
      }                                                                             \
    );                                                                              \
    return true;                                                                    \
  }();                                                                              \
}                                                                                   \
