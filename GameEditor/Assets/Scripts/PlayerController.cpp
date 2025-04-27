    #include "PlayerController.h"
    #include "Rigidbody.h"
    #include "InputManager.h"
    #include "Camera.h"
    #include <GLFW/glfw3.h>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/gtx/quaternion.hpp>

    void PlayerController::Start() {
        // cache the Rigidbody on this GO
        rb = owner->GetComponent<Rigidbody>();
        if (!rb) {
            std::cerr << "[PlayerController] No Rigidbody found on "
                << owner->name << "\n";
        }
        // find a child with a Camera component
        for (auto* child : owner->children) {
            if (child->GetComponent<Camera>()) {
                cameraGO = child;
                break;
            }
        }
    }

    void PlayerController::Update(float dt) {
        // — Mouse look —
        glm::vec2 md = InputManager::Instance().GetMouseDelta();
        // yaw: rotate the player around world-up
        float yaw = md.x * mouseSensitivity;
        glm::quat qy = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
        owner->transform.rotation = qy * owner->transform.rotation;

        // pitch: rotate only the camera child on its local X
        if (cameraGO) {
            pitch -= md.y * mouseSensitivity;
            pitch = glm::clamp(pitch, -89.0f, 89.0f);
            glm::quat qp = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
            cameraGO->transform.rotation = qp;
        }

        // — WASD movement (in the player’s forward/right plane) —
        glm::vec3 dir(0.0f);
        auto& inp = InputManager::Instance();
        if (inp.IsKeyDown(GLFW_KEY_W)) dir += glm::vec3(0, 0, -1);
        if (inp.IsKeyDown(GLFW_KEY_S)) dir += glm::vec3(0, 0, 1);
        if (inp.IsKeyDown(GLFW_KEY_A)) dir += glm::vec3(-1, 0, 0);
        if (inp.IsKeyDown(GLFW_KEY_D)) dir += glm::vec3(1, 0, 0);

        if (rb && glm::length(dir) > 0.0f) {
            dir = glm::normalize(dir);
            // transform into world-space
            glm::vec3 vel = owner->transform.rotation * dir * moveSpeed;
            rb->SetLinearVelocity(vel);
        }
        else if (rb) {
            // stop when no input
            rb->SetLinearVelocity(glm::vec3(0.0f));
        }
    }