#pragma once
#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <nlohmann/json.hpp>
#include <glm.hpp>
#include "GameObject.h"
#include "ComponentFactory.h"
#include "EnginePluginAPI.h"
class ENGINE_API BoxCollider : public ISerializableComponent {
public:
    bool   m_attached = false;
    glm::vec3 halfExtents{ 1,1,1 };
    reactphysics3d::BoxShape* shape = nullptr;

    void Update(float dt) override;
    void OnInspectorGUI() override;
    virtual void OnDrawGizmos(Camera* cam) override;
    nlohmann::json Serialize() const override {
        return {
            {"halfExtents", {halfExtents.x, halfExtents.y, halfExtents.z}}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        auto arr = j.value("halfExtents", std::vector<float>{1, 1, 1});
        halfExtents = { arr[0], arr[1], arr[2] };
    }
};

REGISTER_SERIALIZABLE_COMPONENT(BoxCollider);