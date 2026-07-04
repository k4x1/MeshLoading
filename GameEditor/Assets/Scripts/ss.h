#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class ss : public ISerializableComponent
{
public:
    ss() = default;
    ~ss() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override;
    void Deserialize(const nlohmann::json& j) override;
};

REGISTER_SERIALIZABLE_COMPONENT(ss);
