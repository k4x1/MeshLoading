#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class Enemy : public ISerializableComponent, public ICollisionCallbacks {
public:
    float speed = 3.0f;

    Enemy() = default;
    ~Enemy() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnCollisionEnter(const std::vector<CollisionInfo>& contacts) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override { return { {"speed", speed} }; }
    void Deserialize(const nlohmann::json& j) override { speed = j.value("speed", speed); }

private:
    GameObject* player;
};

REGISTER_SERIALIZABLE_COMPONENT(Enemy);
