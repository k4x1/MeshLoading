#include "InputManager.h"
#include "Debug.h"
InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager() {
    mouseX = 0.0;
    mouseY = 0.0;
    scrollOffset = 0.0;
}

void InputManager::Update() {
    previousKeyStates = currentKeyStates;
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

glm::vec2 InputManager::GetMouseDelta() const
{
    glm::vec2 delta(
        static_cast<float>(mouseX - previousMouseX),
        static_cast<float>(mouseY - previousMouseY)
    );
    Debug::Log(mouseX - previousMouseX);
    return delta;
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


void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        Instance().currentKeyStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        Instance().currentKeyStates[key] = false;
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Instance().previousMouseX = Instance().mouseX;
    Instance().previousMouseY = Instance().mouseY;
    Instance().mouseX = xpos;
    Instance().mouseY = ypos;
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Instance().scrollOffset += yoffset;
}
