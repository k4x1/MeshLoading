#include "BoxCollider.h"
#include "PhysicsEngine.h"
#include "Debug.h"
#include <imgui.h>
#include "Rigidbody.h"

void BoxCollider::OnAttach()
{
    lastScale = glm::vec3(0.0f);
}

void BoxCollider::Update(float dt) {
    glm::vec3 s = owner->transform.scale;
    if (s != lastScale) {
        lastScale = s;

        if (collider) {
            owner->GetComponent<Rigidbody>()->body->removeCollider(collider);
            collider = nullptr;
        }

        reactphysics3d::Vector3 worldHe{
            halfExtents.x * s.x,
            halfExtents.y * s.y,
            halfExtents.z * s.z
        };

        auto& common = Physics::PhysicsEngine::Instance().GetCommon();
        shape = common.createBoxShape(worldHe);
        if (auto* rb = owner->GetComponent<Rigidbody>()) {
            collider = rb->body->addCollider(
                shape, reactphysics3d::Transform::identity());
            int numCols = rb->body->getNbColliders();

        }
    }
}
void BoxCollider::OnInspectorGUI() {
    if (ImGui::DragFloat3("Half Extents", &halfExtents.x, 0.1f)) {
        lastScale = glm::vec3(0.0f);
    }
}
