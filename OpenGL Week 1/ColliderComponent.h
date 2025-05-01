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
    virtual ~ColliderComponent();

    void OnAttach() override;
    void Update(float dt) override;

    virtual void OnInspectorGUI() override = 0;
    virtual void OnDrawGizmos(Camera* cam) override = 0;
};
