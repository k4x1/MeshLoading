#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class PlayerShooter : public ISerializableComponent {
public:
    float shootDistance = 100.0f;
    int   score = 0;

    PlayerShooter() = default;
    ~PlayerShooter() override = default;

    void Update(float dt) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override {
        return { {"shootDistance", shootDistance}, {"score", score} };
    }
    void Deserialize(const nlohmann::json& j) override {
        shootDistance = j.value("shootDistance", shootDistance);
        score = j.value("score", score);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(PlayerShooter);
