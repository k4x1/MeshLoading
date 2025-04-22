#include "Rigidbody.h"
#include "PhysicsEngine.h"
#include <imgui.h>
#include "ComponentFactory.h"
#include "GameObject.h"

void Rigidbody::Start() {
    using namespace reactphysics3d;

    // Build RP3D transform from our GameObject’s Transform
    Vector3       rpPos(
        owner->transform.position.x,
        owner->transform.position.y,
        owner->transform.position.z
    );
    Quaternion    rpRot(
        owner->transform.rotation.x,
        owner->transform.rotation.y,
        owner->transform.rotation.z,
        owner->transform.rotation.w
    );
    Transform     rp3dTransform(rpPos, rpRot);

    // Create the body
    body = PhysicsEngine::Instance().GetWorld()->createRigidBody(rp3dTransform);

    if (isKinematic) {
        body->setType(BodyType::KINEMATIC);
    }
    else {
        body->setMass(mass);
    }
}

void Rigidbody::OnInspectorGUI() {
    ImGui::DragFloat("Mass", &mass, 0.1f);
    ImGui::Checkbox("Kinematic", &isKinematic);
}

nlohmann::json Rigidbody::Serialize(Component* comp) {
    if (auto* rb = dynamic_cast<Rigidbody*>(comp)) {
        return {
            {"mass",        rb->mass},
            {"isKinematic", rb->isKinematic}
        };
    }
    return nullptr;
}

// Register with the factory
static bool _rbReg = []() {
    ComponentFactory::Instance().Register(
        "Rigidbody",
        [](const nlohmann::json& js, GameObject* owner)->Component* {
            auto* rb = owner->AddComponent<Rigidbody>();
            rb->mass = js.value("mass", 1.0f);
            rb->isKinematic = js.value("isKinematic", false);
            return rb;
        },
        [](Component* c) { return Rigidbody::Serialize(c); }
    );
    return true;
    }();
