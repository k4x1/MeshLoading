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
        }
    }
}
void BoxCollider::OnInspectorGUI() {
    if (ImGui::DragFloat3("Half Extents", &halfExtents.x, 0.1f)) {
        lastScale = glm::vec3(0.0f);
    }
}

void BoxCollider::OnDrawGizmos(Camera* cam) {
    if (!cam || !collider) return;

    auto* rb = owner->GetComponent<Rigidbody>();
    auto  bodyXf = rb->body->getTransform();
    auto  localXf = collider->getLocalToBodyTransform();

    auto toMat4 = [](auto& t) {
        glm::quat q{ t.getOrientation().w,
                    t.getOrientation().x,
                    t.getOrientation().y,
                    t.getOrientation().z };
        glm::vec3 p{ t.getPosition().x,
                    t.getPosition().y,
                    t.getPosition().z };
        return glm::translate(glm::mat4(1), p) * glm::mat4_cast(q);
        };
    glm::mat4 worldM = toMat4(bodyXf) * toMat4(localXf);

    Debug::DrawWireBox(worldM,
        { halfExtents.x * lastScale.x,
          halfExtents.y * lastScale.y,
          halfExtents.z * lastScale.z },
        cam);
}