#pragma once

#include "ColliderComponent.h"
#include <nlohmann/json.hpp>
#include "EnginePluginAPI.h"
#include "ComponentFactory.h"

class ENGINE_API SphereCollider : public ColliderComponent {
public:
    float radius = 0.5f;

    nlohmann::json Serialize() const {
        return { {"radius", radius} };
    }

    void Deserialize(const nlohmann::json& j) {
        radius = j.value("radius", radius);
    }


    void OnInspectorGUI() override;

protected:
    void recreateCollider() override;
};

REGISTER_SERIALIZABLE_COMPONENT(SphereCollider);
