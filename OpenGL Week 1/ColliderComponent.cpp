#include "ColliderComponent.h"
#include "Rigidbody.h"

ColliderComponent::ColliderComponent() = default;


void ColliderComponent::OnAttach() {
    lastScale = glm::vec3(0.0f);
}

void ColliderComponent::Update(float dt) {
    glm::vec3 s = owner->transform.scale;
    if (s != lastScale) {
        lastScale = s;
        if (collider) {
            if (auto* rb = owner->GetComponent<Rigidbody>()) {
                rb->body->removeCollider(collider);
            }
            collider = nullptr;
        }
        recreateCollider();
    }
}
