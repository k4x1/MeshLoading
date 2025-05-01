#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "All.h"
#include <nlohmann/json.hpp>
class Rigidbody;
class Camera;

class PlayerController : public ISerializableComponent, public ICollisionCallbacks {
public:
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    float jumpForce = 5.0f;    
    float groundCheckDistance = 1.2f;
    bool grounded = false;
    float yaw = 0.0f;
    float pitch = 0.0f;

    Rigidbody*  rb = nullptr;
    glm::vec3 origin = glm::vec3(0); 
    PlayerController() = default;
    ~PlayerController() override = default;

    void Start() override;
    void FixedUpdate(float fixedDt) override;
    void OnInspectorGUI() override;
    void OnDrawGizmos(Camera* cam) override;

    void OnCollisionEnter(const std::vector<CollisionInfo>& contacts) override;
    void OnCollisionStay(const std::vector<CollisionInfo>& contacts) override;
    void OnCollisionExit(const std::vector<CollisionInfo>& contacts) override;

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