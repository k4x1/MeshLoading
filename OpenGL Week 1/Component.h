#pragma once
#include "IInspectable.h"
#include <nlohmann/json.hpp>

class GameObject;

class Component : public IInspectable {
public:
    GameObject* owner = nullptr;

    virtual void Start() {}
    virtual void Update(float dt) {}
    virtual void Render(class Camera* cam) {}
    virtual void OnInspectorGUI() {}  
    virtual ~Component() {}
};


struct ISerializableComponent : Component {
    virtual nlohmann::json Serialize() const = 0;
    virtual void            Deserialize(const nlohmann::json& j) = 0;
};