#pragma once
#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <glm.hpp>
#include "EnginePluginAPI.h"

class ENGINE_API ColliderComponent : public ISerializableComponent {
protected:
    reactphysics3d::CollisionShape* shape = nullptr;
    reactphysics3d::Collider* collider = nullptr;
    glm::vec3                        lastScale{ 0.0f };

    virtual void recreateCollider() = 0;

public:
    ColliderComponent();

    void OnAttach() override;
    void Update(float dt) override;

    nlohmann::json Serialize() const override { return nullptr; }
    void            Deserialize(const nlohmann::json&) override {}

    virtual void OnInspectorGUI() override {}
    virtual void OnDrawGizmos(Camera* cam) override {}
};
