#include "PlayerController.h"



void PlayerController::Start() {
    rb = owner->GetComponent<Rigidbody>();
    if (!rb) {
        Debug::LogError("no Rigidbody on " + owner->name);
    }
    else {
        Debug::Log("found Rigidbody, ready.");
    }
}

void PlayerController::Update(float dt) {
    if (!rb) return;

    auto& inp = InputManager::Instance();

    glm::vec2 md = inp.GetMouseDelta();


    yaw += md.x * mouseSensitivity;
    pitch -= md.y * mouseSensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::quat qy = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
    glm::quat qp = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    owner->transform.rotation = qy * qp;

    glm::vec3 dir(0.0f);
    if (inp.GetKey(GLFW_KEY_W)) { dir.z -= 1.0f; }
    if (inp.GetKey(GLFW_KEY_S)) { dir.z += 1.0f; }
    if (inp.GetKey(GLFW_KEY_A)) { dir.x -= 1.0f; }
    if (inp.GetKey(GLFW_KEY_D)) { dir.x += 1.0f; }
    if (inp.GetKey(GLFW_KEY_Q)) { dir.y += 1.0f; }
    if (inp.GetKey(GLFW_KEY_E)) { dir.y -= 1.0f; }

    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);

        glm::vec3 forward = owner->transform.rotation * glm::vec3(0, 0, -1);
        glm::vec3 right = owner->transform.rotation * glm::vec3(1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::vec3 worldVel =
            (forward * dir.z +
                right * dir.x +
                up * dir.y)
            * moveSpeed;

        rb->SetLinearVelocity(worldVel);
        
    }
    else {
        rb->SetLinearVelocity(glm::vec3(0.0f));
    }
}

void PlayerController::OnInspectorGUI()
{
    ImGui::DragFloat("MoveSpeed", &moveSpeed, 5.0f);
    ImGui::DragFloat("MouseSensetivity", &mouseSensitivity, 0.01f);
}
