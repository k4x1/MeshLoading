#pragma once

#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <nlohmann/json.hpp>

class Rigidbody : public Component {
public:
    float mass = 1.0f;
    bool  isKinematic = false;
    bool  useGravity = true;
    // RP3D body pointer
    reactphysics3d::RigidBody* body = nullptr;

    void Start() override;
    void OnInspectorGUI() override;

    static nlohmann::json Serialize(Component* comp);
};
