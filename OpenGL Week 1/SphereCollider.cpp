#include "SphereCollider.h"
#include "PhysicsEngine.h"
#include "Rigidbody.h"
#include "Debug.h"
#include <imgui.h>
#include <algorithm>


void SphereCollider::OnInspectorGUI() {
    if (ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 100.0f)) {
        lastScale = glm::vec3(0.0f);
    }
}

void SphereCollider::recreateCollider() {
    auto& common = Physics::PhysicsEngine::Instance().GetCommon();
    glm::vec3 s = owner->transform.scale;
    float worldR = radius * std::max({ s.x, s.y, s.z });
    shape = common.createSphereShape(worldR);
    auto* rb = owner->GetComponent<Rigidbody>();
    collider = rb->body->addCollider(shape, reactphysics3d::Transform::identity());
}


