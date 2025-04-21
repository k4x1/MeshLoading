#define GLM_ENABLE_EXPERIMENTAL
#include "CameraMovement.h"
#include "InputManager.h"  
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>  
#include "GameObject.h"
#include "UIHelpers.h"

CameraMovement::CameraMovement()
    : movementSpeed(100.0f),
    mouseSensitivity(0.1f),
    firstMouse(true),
    lastX(400.0f),
    lastY(400.0f),
    yaw(0.0f),
    pitch(0.0f)
{
}

void CameraMovement::Start() {
    lastX = static_cast<float>(InputManager::Instance().GetMouseX());
    lastY = static_cast<float>(InputManager::Instance().GetMouseY());
    firstMouse = false;

    glm::vec3 euler = glm::degrees(glm::eulerAngles(owner->transform.rotation));
    pitch = euler.x;
    yaw = euler.y; 
}

void CameraMovement::Update(float dt) {
    GLFWwindow* window = InputManager::Instance().GetWindow();

    // read raw button state every frame
    bool rightDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    // —— begin capture if freshly pressed *and* mouse is over a view window
    if (!capturingMouse && rightDown &&
        (UIHelpers::g_SceneViewHovered || UIHelpers::g_GameViewHovered))
    {
        capturingMouse = true;
        // lock and hide cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // seed the lastX/Y so we don't get a giant jump
        lastX = static_cast<float>(InputManager::Instance().GetMouseX());
        lastY = static_cast<float>(InputManager::Instance().GetMouseY());
    }

    // —— if we’re capturing, handle movement and release
    if (capturingMouse) {
        if (rightDown) {
            // still holding: do your existing mouse‐look
            double currX = InputManager::Instance().GetMouseX();
            double currY = InputManager::Instance().GetMouseY();

            float xoffset = float(currX - lastX) * mouseSensitivity;
            float yoffset = float(lastY - currY) * mouseSensitivity;

            lastX = float(currX);
            lastY = float(currY);

            yaw += xoffset;
            pitch -= yoffset;
            pitch = glm::clamp(pitch, -89.0f, 89.0f);

            owner->transform.rotation =
                glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0)) *
                glm::angleAxis(glm::radians(-pitch), glm::vec3(1, 0, 0));
        }
        else {
            // just released: stop capturing
            capturingMouse = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (capturingMouse) {
        glm::vec3 forward = owner->transform.GetForward();
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 up = owner->transform.GetUp();

        if (InputManager::Instance().GetKey(GLFW_KEY_W)) owner->transform.position += forward * movementSpeed * dt;
        if (InputManager::Instance().GetKey(GLFW_KEY_S)) owner->transform.position -= forward * movementSpeed * dt;
        if (InputManager::Instance().GetKey(GLFW_KEY_A)) owner->transform.position += right * movementSpeed * dt;
        if (InputManager::Instance().GetKey(GLFW_KEY_D)) owner->transform.position -= right * movementSpeed * dt;
        if (InputManager::Instance().GetKey(GLFW_KEY_Q)) owner->transform.position += up * movementSpeed * dt;
        if (InputManager::Instance().GetKey(GLFW_KEY_E)) owner->transform.position -= up * movementSpeed * dt;
    }
}
 void CameraMovement::OnInspectorGUI() {
    ImGui::DragFloat("Movement Speed", &movementSpeed, 0.1f);
    ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity, 0.01f);
}