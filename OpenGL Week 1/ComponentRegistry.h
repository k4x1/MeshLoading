#pragma once
#include <functional>
#include <map>
#include <string>
#include "Component.h"

class ComponentRegistry {
public:
    using CreatorFunc = std::function<Component* ()>;

    static ComponentRegistry& Instance() {
        static ComponentRegistry instance;
        return instance;
    }

    void RegisterComponent(const std::string& name, CreatorFunc func) {
        registry[name] = func;
    }

    const std::map<std::string, CreatorFunc>& GetRegistry() const {
        return registry;
    }

private:
    std::map<std::string, CreatorFunc> registry;
};

// A helper macro to register a component type, e.g.,
// REGISTER_COMPONENT(CameraMovement);
#define REGISTER_COMPONENT(ComponentType) \
    ComponentRegistry::Instance().RegisterComponent(#ComponentType, []() -> Component* { return new ComponentType(); });
