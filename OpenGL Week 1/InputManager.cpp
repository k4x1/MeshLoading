#include "InputManager.h"
#include "Scene.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Scene.h"
#include "Component.h"
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>

// Define the singleton instance
InputManager* InputManager::m_instance = nullptr;

InputManager::InputManager(Camera* _camRef, Scene* _scene)
{
    m_camera = _camRef;
    m_scene = _scene;
    m_instance = this;

}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
       // ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        return;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        cursorVisible = !cursorVisible;
        glfwSetInputMode(window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        static bool wireframe = false;
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Cursor Position: (" << xpos << ", " << ypos << ")" << std::endl;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        currentScene = SceneType::Game;
        sceneChanged = true;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        currentScene = SceneType::HeightMap;
        sceneChanged = true;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        currentScene = SceneType::Noise;
        sceneChanged = true;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        currentScene = SceneType::FrameBuffer;
        sceneChanged = true;
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static float lastX = 400, lastY = 400;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    const float sensitivity = 0.1f;
    float xoffset = (xpos - lastX) * sensitivity;
    float yoffset = (lastY - ypos) * sensitivity;  

    lastX = xpos;
    lastY = ypos;

    float yaw = m_camera->owner->transform.rotation.y;
    float pitch = m_camera->owner->transform.rotation.x;

    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    m_camera->owner->transform.rotation.y = yaw;
    m_camera->owner->transform.rotation.x = pitch;
}

void InputManager::ProcessInput(GLFWwindow* window)
{
    m_currentFrame = glfwGetTime();
    m_deltaTime = m_currentFrame - m_lastFrame;
    m_lastFrame = m_currentFrame;

    float pitch = m_camera->owner->transform.rotation.x;
    float yaw = m_camera->owner->transform.rotation.y;
    glm::vec3 forward;
    forward.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    forward.y = sin(glm::radians(pitch));
    forward.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    forward = glm::normalize(forward);

    const float speed = 50.0f;

    // Move forward and backward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_camera->owner->transform.position += forward * speed * static_cast<float>(m_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_camera->owner->transform.position -= forward * speed * static_cast<float>(m_deltaTime);
    }

    // Compute the right vector using cross product (using the camera's up vector)
    glm::vec3 right = glm::normalize(glm::cross(forward, m_camera->m_up));

    // Move left and right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_camera->owner->transform.position -= right * speed * static_cast<float>(m_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_camera->owner->transform.position += right * speed * static_cast<float>(m_deltaTime);
    }

    // Move up and down (using the camera's up vector)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_camera->owner->transform.position -= m_camera->m_up * speed * static_cast<float>(m_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        m_camera->owner->transform.position += m_camera->m_up * speed * static_cast<float>(m_deltaTime);
    }

    // Process scene saving with CTRL+S
    static bool prevSKey = false;
    bool currSKey = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    bool ctrlPressed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    if (currSKey)
    {
        if (ctrlPressed && m_scene != nullptr && !prevSKey)
        {
            m_scene->SaveToFile(m_scene->sceneName + ".json");
        }
    }
    prevSKey = currSKey;
}

void InputManager::SetCursorPosCallback(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, StaticMouseCallback);
    glfwSetScrollCallback(window, StaticScrollCallback);
    glfwSetKeyCallback(window, StaticKeyCallback);
}

void InputManager::StaticScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (m_instance) {
    
        m_instance->ScrollCallback(window, xoffset, yoffset);
    }
}

void InputManager::StaticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (m_instance) {
        m_instance->MouseCallback(window, xpos, ypos);
    }
}

void InputManager::StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (m_instance) {
        m_instance->KeyCallback(window, key, scancode, action, mods);
    }
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse)
    {
      //  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        return;

    }
    m_camera->m_FOV -= static_cast<float>(yoffset);
    if (m_camera->m_FOV < 1.0f)
        m_camera->m_FOV = 1.0f;
    if (m_camera->m_FOV > 179.0f)
        m_camera->m_FOV = 179.0f;
}
