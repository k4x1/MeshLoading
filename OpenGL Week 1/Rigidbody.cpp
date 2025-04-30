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
    body->setUserData(static_cast<void*>(owner));
    body->enableGravity(useGravity);
    if (isKinematic) {
        body->setType(BodyType::KINEMATIC);
    }
    else
    {
        body->setMass(mass);
    }

}

void Rigidbody::FixedUpdate(float fixedDt) {
    if (!body) return;
    if (body->getType() == rp3d::BodyType::KINEMATIC) {
        rp3d::Transform newXf(
            rp3d::Vector3{ owner->transform.position.x,
                           owner->transform.position.y,
                           owner->transform.position.z },
            rp3d::Quaternion{ owner->transform.rotation.x,
                              owner->transform.rotation.y,
                              owner->transform.rotation.z,
                              owner->transform.rotation.w }
        );
        body->setTransform(newXf);  
    }
    else{
        auto rp3dTransform = body->getTransform();
        auto& rpPos = rp3dTransform.getPosition();
        auto& rpOri = rp3dTransform.getOrientation();

        glm::vec3 newPos(rpPos.x, rpPos.y, rpPos.z);
        glm::quat newRot(rpOri.w, rpOri.x, rpOri.y, rpOri.z);

        glm::vec3 oldPos = owner->transform.position;
        owner->transform.position.x = lockPosX ? oldPos.x : newPos.x;
        owner->transform.position.y = lockPosY ? oldPos.y : newPos.y;
        owner->transform.position.z = lockPosZ ? oldPos.z : newPos.z;
    }
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
    ImGui::Separator();
    ImGui::Text("Lock Transform:");
    ImGui::Indent();
    ImGui::Checkbox("Position X", &lockPosX);
    ImGui::Checkbox("Position Y", &lockPosY);
    ImGui::Checkbox("Position Z", &lockPosZ);
    ImGui::Checkbox("Rotation X", &lockRotX);
    ImGui::Checkbox("Rotation Y", &lockRotY);
    ImGui::Checkbox("Rotation Z", &lockRotZ);
    ImGui::Unindent();
}

