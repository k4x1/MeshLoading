#pragma once
#include "IInspectable.h"
#include <nlohmann/json.hpp>
#include "EnginePluginAPI.h"

class GameObject;

class ENGINE_API Component : public IInspectable {
public:
    GameObject* owner = nullptr;
    Component() = default;

    virtual void Start() {}
    virtual void Update(float dt) {}
    virtual void Render(class Camera* cam) {}
    virtual void OnInspectorGUI() {}  
    virtual ~Component() {}
};


class ENGINE_API ISerializableComponent : public Component {
    virtual nlohmann::json Serialize() const = 0;
    virtual void            Deserialize(const nlohmann::json& j) = 0;
};