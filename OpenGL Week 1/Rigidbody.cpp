#include "Rigidbody.h"
#include "PhysicsEngine.h"
#include <imgui.h>

void Rigidbody::Start() {
    using namespace reactphysics3d;
    Vector3 rpPos(
        owner->transform.position.x,
        owner->transform.position.y,
        owner->transform.position.z
    );
    Quaternion rpRot(
        owner->transform.rotation.x,
        owner->transform.rotation.y,
        owner->transform.rotation.z,
        owner->transform.rotation.w
    );
    reactphysics3d::Transform xform(rpPos, rpRot);
    body =  Physics::PhysicsEngine::Instance().GetWorld()->createRigidBody(xform);
    body->enableGravity(useGravity);
    if (isKinematic)
        body->setType(BodyType::KINEMATIC);
    else
        body->setMass(mass);
}

void Rigidbody::Update(float dt) {
    if (!body) return;

    auto rp3dTransform = body->getTransform();
    auto& rpPos = rp3dTransform.getPosition();
    auto& rpOri = rp3dTransform.getOrientation();

    owner->transform.position = glm::vec3(rpPos.x, rpPos.y, rpPos.z);
    owner->transform.rotation = glm::quat(rpOri.w, rpOri.x, rpOri.y, rpOri.z);
}
void Rigidbody::SetLinearVelocity(const glm::vec3& velocity) const
{
    body->setLinearVelocity(reactphysics3d::Vector3(velocity.x,velocity.y,velocity.z));
}
void Rigidbody::OnInspectorGUI() {
    ImGui::DragFloat("Mass", &mass, 0.1f);
    ImGui::Checkbox("Kinematic", &isKinematic);
    ImGui::Checkbox("Use Gravity", &useGravity);
    if (body) body->enableGravity(useGravity);
}

