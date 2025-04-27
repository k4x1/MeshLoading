#pragma once

#include "Component.h"
#include <nlohmann/json.hpp>

class Rigidbody;
class Camera;

class PlayerController : public ISerializableComponent {
public:
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.1f;

    // — runtime state (not serialized) —
    float        pitch = 0.0f;
    Rigidbody* rb = nullptr;
    GameObject* cameraGO = nullptr;

    PlayerController() = default;
    ~PlayerController() override = default;

    void Start() override;
    void Update(float dt) override;

    // ─── Serialization ────────────────────────────────────────────
    nlohmann::json Serialize() const override {
        return {
            {"moveSpeed",        moveSpeed},
            {"mouseSensitivity", mouseSensitivity}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        moveSpeed = j.value("moveSpeed", moveSpeed);
        mouseSensitivity = j.value("mouseSensitivity", mouseSensitivity);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(PlayerController);