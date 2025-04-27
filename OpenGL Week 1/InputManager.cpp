#include "InputManager.h"

// Define the singleton instance via a static local variable in Instance()
InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager() {
    // Initialize default values
    mouseX = 0.0;
    mouseY = 0.0;
    scrollOffset = 0.0;
}

void InputManager::Update() {
    // Copy current key states into previous key states for transition detection.
    previousKeyStates = currentKeyStates;
    // Reset scroll offset after processing.
    scrollOffset = 0.0;
}

bool InputManager::GetKey(int key) const {
    auto it = currentKeyStates.find(key);
    return (it != currentKeyStates.end()) ? it->second : false;
}

bool InputManager::GetKeyDown(int key) const {
    bool curr = GetKey(key);
    bool prev = false;
    auto it = previousKeyStates.find(key);
    if (it != previousKeyStates.end()) {
        prev = it->second;
    }
    return curr && !prev;
}

bool InputManager::GetKeyUp(int key) const {
    bool curr = GetKey(key);
    bool prev = false;
    auto it = previousKeyStates.find(key);
    if (it != previousKeyStates.end()) {
        prev = it->second;
    }
    return !curr && prev;
}

double InputManager::GetMouseX() const {
    return mouseX;
}

double InputManager::GetMouseY() const {
    return mouseY;
}

void InputManager::GetMousePosition(double& x, double& y) const {
    x = mouseX;
    y = mouseY;
}

double InputManager::GetScrollOffset() const {
    return scrollOffset;
}

void InputManager::SetCallbacks(GLFWwindow* window) {
    m_window = window;
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

// Static callback implementations:

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Here you may check for ImGui capture if needed.
    if (action == GLFW_PRESS) {
        Instance().currentKeyStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        Instance().currentKeyStates[key] = false;
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Instance().mouseX = xpos;
    Instance().mouseY = ypos;
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Instance().scrollOffset += yoffset;
}
