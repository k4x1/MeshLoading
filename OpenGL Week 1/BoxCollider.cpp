#include "BoxCollider.h"
#include "PhysicsEngine.h"
#include "Rigidbody.h"
#include "Debug.h"
#include <imgui.h>



void BoxCollider::OnInspectorGUI() {
    if (ImGui::DragFloat3("Half Extents", &halfExtents.x, 0.1f)) {
        lastScale = glm::vec3(0.0f);
    }
}

void BoxCollider::recreateCollider() {
    auto& common = Physics::PhysicsEngine::Instance().GetCommon();
    glm::vec3 s = owner->transform.scale;
    reactphysics3d::Vector3 worldHe{
        halfExtents.x * s.x,
        halfExtents.y * s.y,
        halfExtents.z * s.z
    };
    shape = common.createBoxShape(worldHe);
    auto* rb = owner->GetComponent<Rigidbody>();
    collider = rb->body->addCollider(shape, reactphysics3d::Transform::identity());
}

