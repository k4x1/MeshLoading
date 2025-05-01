#pragma once

#include "ColliderComponent.h"
#include "ComponentFactory.h"
#include <nlohmann/json.hpp>
#include <glm.hpp>
#include "EnginePluginAPI.h"

class ENGINE_API BoxCollider : public ColliderComponent {
public:
    glm::vec3 halfExtents{ 1,1,1 };

    nlohmann::json Serialize() const {
        return { {"halfExtents", {halfExtents.x, halfExtents.y, halfExtents.z}} };
    }
    void Deserialize(const nlohmann::json& j) {
        auto a = j.value("halfExtents", std::vector<float>{1, 1, 1});
        halfExtents = { a[0], a[1], a[2] };
    }

    void OnInspectorGUI() override;


protected:
    void recreateCollider() override;
};

REGISTER_SERIALIZABLE_COMPONENT(BoxCollider);
