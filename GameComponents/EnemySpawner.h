#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class EnemySpawner : public ISerializableComponent {
public:
    std::string prefabPath = "Assets/Prefabs/Enemy.prefab";
    float       spawnInterval = 100.0f;
    int         maxEnemies = 10;

    EnemySpawner() = default;
    ~EnemySpawner() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override {
        return { {"prefabPath", prefabPath},
            {"spawnInterval", spawnInterval},
            {"maxEnemies", maxEnemies} };
    }
    void Deserialize(const nlohmann::json& j) override {
        prefabPath = j.value("prefabPath", prefabPath);
        spawnInterval = j.value("spawnInterval", spawnInterval);
        maxEnemies = j.value("maxEnemies", maxEnemies);
    }
private:
    float timer = 0.0f;
    int   spawnedCount = 0;
};

REGISTER_SERIALIZABLE_COMPONENT(EnemySpawner);
