#define GLM_ENABLE_EXPERIMENTAL
#include "CameraMovement.h"
#include "InputManager.h"  
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>  
#include "GameObject.h"

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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        double currentX = InputManager::Instance().GetMouseX();
        double currentY = InputManager::Instance().GetMouseY();

        float xoffset = static_cast<float>(currentX - lastX) * mouseSensitivity;
        float yoffset = static_cast<float>(lastY - currentY) * mouseSensitivity;

        lastX = static_cast<float>(currentX);
        lastY = static_cast<float>(currentY);

        yaw += xoffset;
        pitch -= yoffset;

        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        owner->transform.rotation =
            glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0)) *
            glm::angleAxis(glm::radians(-pitch), glm::vec3(1, 0, 0));

    }   
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        lastX = static_cast<float>(InputManager::Instance().GetMouseX());
        lastY = static_cast<float>(InputManager::Instance().GetMouseY());
    }

    glm::vec3 forward = owner->transform.GetForward();
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = owner->transform.GetUp();

    if (InputManager::Instance().GetKey(GLFW_KEY_W))
        owner->transform.position += forward * movementSpeed * dt;
    if (InputManager::Instance().GetKey(GLFW_KEY_S))
        owner->transform.position -= forward * movementSpeed * dt;
    if (InputManager::Instance().GetKey(GLFW_KEY_A))
        owner->transform.position -= right * movementSpeed * dt;
    if (InputManager::Instance().GetKey(GLFW_KEY_D))
        owner->transform.position += right * movementSpeed * dt;
    if (InputManager::Instance().GetKey(GLFW_KEY_Q))
        owner->transform.position -= up * movementSpeed * dt;
    if (InputManager::Instance().GetKey(GLFW_KEY_E))
        owner->transform.position += up * movementSpeed * dt;
}
