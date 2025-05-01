#include "CapsuleCollider.h"
#include "PhysicsEngine.h"
#include "Rigidbody.h"
#include "Debug.h"
#include <imgui.h>
#include <algorithm>


void CapsuleCollider::OnInspectorGUI() {
    if (ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 100.0f) ||
        ImGui::DragFloat("Height", &height, 0.01f, 0.0f, 100.0f)) {
        lastScale = glm::vec3(0.0f);
    }
}

void CapsuleCollider::recreateCollider() {
    auto& common = Physics::PhysicsEngine::Instance().GetCommon();
    glm::vec3 s = owner->transform.scale;
    float worldR = radius * std::max({ s.x, s.z });
    float worldH = height * s.y;
    shape = common.createCapsuleShape(worldR, worldH);
    auto* rb = owner->GetComponent<Rigidbody>();
    collider = rb->body->addCollider(shape, reactphysics3d::Transform::identity());
}

