#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "All.h"
#include <nlohmann/json.hpp>
class Rigidbody;
class Camera;

class PlayerController : public ISerializableComponent {
public:
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    float jumpForce = 5.0f;    
    float groundCheckDistance = 1.1f;

    float yaw = 0.0f;
    float pitch = 0.0f;

    Rigidbody*  rb = nullptr;

    PlayerController() = default;
    ~PlayerController() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnInspectorGUI() override;
    nlohmann::json Serialize() const override {
        return {
            {"moveSpeed",           moveSpeed},
            {"mouseSensitivity",    mouseSensitivity},
            {"jumpForce",           jumpForce},
            {"groundCheckDistance", groundCheckDistance}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        moveSpeed =           j.value("moveSpeed", moveSpeed);
        mouseSensitivity =    j.value("mouseSensitivity", mouseSensitivity);
        jumpForce =           j.value("jumpForce", jumpForce);
        groundCheckDistance = j.value("groundCheckDistance", groundCheckDistance);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(PlayerController);