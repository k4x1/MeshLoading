#include "BoxCollider.h"
#include "PhysicsEngine.h"
#include "Rigidbody.h"
#include "Debug.h"
#include <imgui.h>
#include "ComponentFactory.h"
#include "GameObject.h"
void BoxCollider::Start() {
    auto* rb = owner->GetComponent<Rigidbody>();
    if (!rb) return;
    auto& common = PhysicsEngine::Instance().GetCommon();
    shape = common.createBoxShape(
        reactphysics3d::Vector3{ halfExtents.x, halfExtents.y, halfExtents.z });
    rb->body->addCollider(shape, reactphysics3d::Transform::identity());
}

void BoxCollider::OnInspectorGUI() {
    ImGui::DragFloat3("Half Extents", &halfExtents.x, 0.1f);
}

void BoxCollider::Render(Camera* cam) {
    Debug::DrawWireBox(owner->GetWorldMatrix(), halfExtents, cam);
}

nlohmann::json BoxCollider::Serialize(Component* comp) {
    auto* bc = dynamic_cast<BoxCollider*>(comp);
    if (!bc) return nullptr;
    return {
        {"halfExtents",{bc->halfExtents.x, bc->halfExtents.y, bc->halfExtents.z}}
    };
}

// Registration
static bool _bcReg = []() {
    ComponentFactory::Instance().Register(
        "BoxCollider",
        // deserializer
        [](const nlohmann::json& js, GameObject* owner)->Component* {
            auto* bc = owner->AddComponent<BoxCollider>();
            auto  v = js.value("halfExtents", std::vector<float>{1, 1, 1});
            bc->halfExtents = { v[0], v[1], v[2] };
            return bc;
        },
        // serializer
        [](Component* c) { return BoxCollider::Serialize(c); }
    );
    return true;
    }();