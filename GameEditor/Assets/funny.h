#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class funny : public ISerializableComponent
{
public:
    funny() = default;
    ~funny() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override;
    void Deserialize(const nlohmann::json& j) override;
};

REGISTER_SERIALIZABLE_COMPONENT(funny);
