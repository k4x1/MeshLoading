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

    bool lockPosX = false, lockPosY = false, lockPosZ = false;
    bool lockRotX = false, lockRotY = false, lockRotZ = false;

    reactphysics3d::RigidBody* body = nullptr;

    void Start() override;
    void FixedUpdate(float fixedDt) override;
    void Render(class Camera*) override {}
    void OnInspectorGUI() override;
    void SetLinearVelocity(const glm::vec3& velocity) const;

    nlohmann::json Serialize() const override {
        return {
            {"mass",        mass},
            {"isKinematic", isKinematic},
            {"useGravity",  useGravity},
            { "lockPosX", lockPosX }, {"lockPosY", lockPosY}, {"lockPosZ", lockPosZ},
            {"lockRotX", lockRotX}, {"lockRotY", lockRotY}, {"lockRotZ", lockRotZ}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        mass = j.value("mass", mass);
        isKinematic = j.value("isKinematic", isKinematic);
        useGravity = j.value("useGravity", useGravity);
        lockPosX = j.value("lockPosX", lockPosX);
        lockPosY = j.value("lockPosY", lockPosY);
        lockPosZ = j.value("lockPosZ", lockPosZ);
        lockRotX = j.value("lockRotX", lockRotX);
        lockRotY = j.value("lockRotY", lockRotY);
        lockRotZ = j.value("lockRotZ", lockRotZ);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(Rigidbody);