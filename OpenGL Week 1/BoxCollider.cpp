#include "BoxCollider.h"
#include "PhysicsEngine.h"
#include "Debug.h"
#include <imgui.h>
#include "Rigidbody.h"

void BoxCollider::Update(float dt)
{
    if (!m_attached) {
        auto* rb = owner->GetComponent<Rigidbody>();
        if (rb && rb->body) {
            auto& common = Physics::PhysicsEngine::Instance().GetCommon();
            shape = common.createBoxShape(
                reactphysics3d::Vector3{ halfExtents.x, halfExtents.y, halfExtents.z });
            rb->body->addCollider(shape, reactphysics3d::Transform::identity());
            m_attached = true;
        }
    }
}


void BoxCollider::OnInspectorGUI() {
    ImGui::DragFloat3("Half Extents", &halfExtents.x, 0.1f);
}

void BoxCollider::OnDrawGizmos(Camera* cam) {
    Debug::DrawWireBox(owner->GetWorldMatrix(), halfExtents, cam);
}