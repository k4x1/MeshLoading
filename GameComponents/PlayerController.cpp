#include "PlayerController.h"


    void PlayerController::Start() {
        rb = owner->GetComponent<Rigidbody>();
        if (!rb) {
            std::cerr << "[PlayerController] No Rigidbody found on "
                << owner->name << "\n";
        }
        for (auto* child : owner->children) {
            if (child->GetComponent<Camera>()) {
                cameraGO = child;
                break;
            }
        }
    }

    void PlayerController::Update(float dt) {
        glm::vec2 md = InputManager::Instance().GetMouseDelta();
        float yaw = md.x * mouseSensitivity;
        glm::quat qy = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
        owner->transform.rotation = qy * owner->transform.rotation;

        if (cameraGO) {
            pitch -= md.y * mouseSensitivity;
            pitch = glm::clamp(pitch, -89.0f, 89.0f);
            glm::quat qp = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
            cameraGO->transform.rotation = qp;
        }

        glm::vec3 dir(0.0f);
        auto& inp = InputManager::Instance();
        if (inp.GetKeyDown(GLFW_KEY_W)) dir += glm::vec3(0, 0, -1);
        if (inp.GetKeyDown(GLFW_KEY_S)) dir += glm::vec3(0, 0, 1);
        if (inp.GetKeyDown(GLFW_KEY_A)) dir += glm::vec3(-1, 0, 0);
        if (inp.GetKeyDown(GLFW_KEY_D)) dir += glm::vec3(1, 0, 0);

        if (rb && glm::length(dir) > 0.0f) {
            dir = glm::normalize(dir);
            glm::vec3 vel = owner->transform.rotation * dir * moveSpeed;
            rb->SetLinearVelocity(vel);
        }
        else if (rb) {
            rb->SetLinearVelocity(glm::vec3(0.0f));
        }
    }