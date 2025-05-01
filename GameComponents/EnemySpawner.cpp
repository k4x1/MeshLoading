#include "EnemySpawner.h"
#include "GameObject.h"
#include "PrefabSystem.h"
#include "Debug.h"


void EnemySpawner::Start() {
    timer = 0.0f;
    spawnedCount = 0;
    DEBUG_LOG("EnemySpawner: will spawn \"" << prefabPath
        << "\" every " << spawnInterval
        << "s, up to " << maxEnemies);
}

void EnemySpawner::Update(float dt) {
    if (spawnedCount >= maxEnemies) return;

    timer += dt;
    if (timer >= spawnInterval) {
        timer -= spawnInterval;  
        GameObject* enemy = PrefabSystem::Instantiate(prefabPath);
        if (!enemy) {
            DEBUG_ERR("EnemySpawner: failed to instantiate prefab at " << prefabPath);
            return;
        }
        if (owner->GetScene()) {
            owner->GetScene()->AddGameObject(enemy);
        //    enemy->Start();
            DEBUG_LOG("EnemySpawner: spawned \"" << enemy->name << "\"");
            spawnedCount++;
        }
        else {
            DEBUG_ERR("EnemySpawner: owner->scene is null!");
            delete enemy;
        }
    }
}

void EnemySpawner::OnInspectorGUI() {
    static char prefabBuf[256];
    strncpy_s(prefabBuf, prefabPath.c_str(), sizeof(prefabBuf));
    prefabBuf[sizeof(prefabBuf) - 1] = '\0';

    if (ImGui::InputText("Prefab Path", prefabBuf, sizeof(prefabBuf))) {
        prefabPath = prefabBuf;
    }

    ImGui::DragFloat("Spawn Interval", &spawnInterval, 0.1f, 0.1f, 60.0f);
    ImGui::DragInt("Max Enemies", &maxEnemies, 1, 1, 100);
}
