#include "PlayerController.h"



void PlayerController::Start() {
    rb = owner->GetComponent<Rigidbody>();
    if (!rb) {
        Debug::LogError("no Rigidbody on " + owner->name);
    }   
}

void PlayerController::FixedUpdate(float fixedDt) {
    if (!rb || !rb->body) return;

    auto& inp = InputManager::Instance();
    glm::vec2 md = inp.GetMouseDelta();
    yaw += md.x * mouseSensitivity;
    pitch -= md.y * mouseSensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    glm::quat qy = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
    glm::quat qp = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    owner->transform.rotation = qy * qp;

    glm::vec3 dir(0.0f);
    if (inp.GetKey(GLFW_KEY_W)) dir.z += 1.0f;
    if (inp.GetKey(GLFW_KEY_S)) dir.z += -1.0f;
    if (inp.GetKey(GLFW_KEY_A)) dir.x += 1.0f;
    if (inp.GetKey(GLFW_KEY_D)) dir.x += -1.0f;

    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);
        glm::vec3 forward = owner->transform.rotation * glm::vec3(0, 0, -1);
        glm::vec3 right = owner->transform.rotation * glm::vec3(1, 0, 0);
        glm::vec3 worldDir = forward * dir.z + right * dir.x;

        rb->body->applyWorldForceAtCenterOfMass(
            reactphysics3d::Vector3(worldDir.x * moveSpeed,
                0,
                worldDir.z * moveSpeed));
    }

    auto rpVel = rb->body->getLinearVelocity();
    glm::vec3 newVel(rpVel.x, rpVel.y, rpVel.z);

    Physics::RaycastHit hit;
    bool grounded = Physics::PhysicsEngine::Instance().Raycast(
        owner->transform.position,
        glm::vec3(0, -1, 0),
        hit,
        groundCheckDistance);

    if (grounded && inp.GetKeyDown(GLFW_KEY_SPACE)) {
        newVel.y = jumpForce;
    }

    rb->body->setLinearVelocity(
        reactphysics3d::Vector3(newVel.x, newVel.y, newVel.z));
}

void PlayerController::OnCollisionEnter(const std::vector<CollisionInfo>& contacts) {
    //Debug::Log("PlayerController: OnCollisionEnter with " + std::to_string(contacts.size()) + " points");
    //for (auto& ci : contacts) {
    //    Debug::Log("   hit: " + ci.other->name
    //        + "  pt("
    //        + std::to_string(ci.contactPoint.x) + ","
    //        + std::to_string(ci.contactPoint.y) + ","
    //        + std::to_string(ci.contactPoint.z) + ")");
    //}
}

void PlayerController::OnCollisionStay(const std::vector<CollisionInfo>& contacts) {
 //   Debug::Log("PlayerController: OnCollisionStay");
}

void PlayerController::OnCollisionExit(const std::vector<CollisionInfo>& contacts) {
 //   Debug::Log("PlayerController: OnCollisionExit");
}
void PlayerController::OnInspectorGUI() {
    ImGui::DragFloat("Move Speed", &moveSpeed, 1.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Jump Force", &jumpForce, 0.5f, 0.0f, 50.0f);
    ImGui::DragFloat("Ground Check Dist", &groundCheckDistance, 0.1f, 0.1f, 5.0f);
}