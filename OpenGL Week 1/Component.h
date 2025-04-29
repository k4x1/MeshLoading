#pragma once
#include "IInspectable.h"
#include <nlohmann/json.hpp>
#include "EnginePluginAPI.h"

class GameObject;

class ENGINE_API Component : public IInspectable {
public:
    GameObject* owner = nullptr;
    Component() = default;
    virtual std::string GetName() const {
        std::string n = typeid(*this).name();
        constexpr char prefix[] = "class ";
        if (n.rfind(prefix, 0) == 0)
            n.erase(0, sizeof(prefix) - 1);
        return n;
    }

    virtual void Start() {}
    virtual void Update(float dt) {}
    virtual void FixedUpdate(float fixedDt) {}
    virtual void Render(class Camera* cam) {}
    virtual void OnInspectorGUI() {}  
    virtual ~Component() {}
};


class ENGINE_API ISerializableComponent : public Component {
    virtual nlohmann::json Serialize() const = 0;
    virtual void            Deserialize(const nlohmann::json& j) = 0;
};