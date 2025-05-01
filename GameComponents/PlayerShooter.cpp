#include "PlayerShooter.h"
#include "InputManager.h"
#include "PhysicsEngine.h"
#include "TagComponent.h"
#include "GameObject.h"
#include "Debug.h"

void PlayerShooter::Update(float dt) {
    if (InputManager::Instance().GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        auto* camComp = owner->GetComponent<Camera>();
        glm::vec3 origin = owner->transform.position;
        glm::vec3 forward = owner->transform.GetForward();

        Physics::RaycastHit hit;
        if (Physics::PhysicsEngine::Instance().Raycast(origin, forward, hit, shootDistance)) {
            if (auto* go = hit.object) {
                auto* tag = go->GetComponent<TagComponent>();
                if (tag && tag->tag == "Enemy") {
                    score += 1;
                    DEBUG_LOG("Enemy hit! Score = " << score);
                    delete go;
                }
            }
        }
    }
}

void PlayerShooter::OnInspectorGUI() {
    ImGui::DragFloat("Shoot Distance", &shootDistance, 1.0f, 0.0f, 1000.0f);
    ImGui::Text("Score: %d", score);
}
