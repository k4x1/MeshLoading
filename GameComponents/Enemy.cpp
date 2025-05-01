#include "Enemy.h"
#include "GameObject.h"
#include "PlayerShooter.h"
#include "Debug.h"

void Enemy::Start()
{
    auto* s = owner->GetScene();
    if (!s) {
        DEBUG_ERR("Enemy::Start(): scene is null!");
        return;
    }
    for (auto* go : s->gameObjects) {
        if (go->GetComponent<PlayerShooter>()) {
            player = go;
            break;
        }
    }
}

void Enemy::Update(float dt) {
   
    if (!player) return;

    glm::vec3 dir = glm::normalize(player->transform.position - owner->transform.position);
    owner->transform.position += dir * speed * dt;
}

void Enemy::OnCollisionEnter(const std::vector<CollisionInfo>&) {

  /*  for (GameObject* go : owner->GetScene()->gameObjects) {
        if (auto* ps = go->GetComponent<PlayerShooter>()) {
            ps->score -= 1;
            DEBUG_LOG("Player hit! Score = " << ps->score);
            break;  
        }
    }

    owner->GetScene()->RemoveGameObject(owner);*/
}

void Enemy::OnInspectorGUI()
{
    ImGui::DragFloat("Speed", &speed, 3.0f);
}
