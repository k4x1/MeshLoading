#pragma once

#include "ColliderComponent.h"
#include <nlohmann/json.hpp>
#include "EnginePluginAPI.h"
#include "ComponentFactory.h"

class ENGINE_API CapsuleCollider : public ColliderComponent {
public:
    float radius = 0.5f;
    float height = 2.0f;

    nlohmann::json Serialize() const {
        return { {"radius", radius}, {"height", height} };
    }

    void Deserialize(const nlohmann::json& j) {
        radius = j.value("radius", radius);
        height = j.value("height", height);
    }


    void OnInspectorGUI() override;

protected:
    void recreateCollider() override;
};

REGISTER_SERIALIZABLE_COMPONENT(CapsuleCollider);
