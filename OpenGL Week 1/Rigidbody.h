#pragma once

#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <nlohmann/json.hpp>
#include "GameObject.h"
#include "ComponentFactory.h"

class ENGINE_API Rigidbody : public ISerializableComponent {
public:
    float mass = 1.0f;
    bool  isKinematic = false;
    bool  useGravity = true;
    reactphysics3d::RigidBody* body = nullptr;

    void Start() override;
    void Update(float) override;
    void Render(class Camera*) override {}
    void OnInspectorGUI() override;
    void SetLinearVelocity(const glm::vec3& velocity) const;

    nlohmann::json Serialize() const override {
        return {
            {"mass",        mass},
            {"isKinematic", isKinematic},
            {"useGravity",  useGravity}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        mass = j.value("mass", mass);
        isKinematic = j.value("isKinematic", isKinematic);
        useGravity = j.value("useGravity", useGravity);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(Rigidbody);