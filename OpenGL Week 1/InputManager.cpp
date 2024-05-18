/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : InputManager.cpp
Description : Implementation file for the InputManager class, which handles keyboard and mouse input for the application.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#include "InputManager.h"

// Initialize the singleton instance
InputManager* InputManager::m_instance = nullptr;

// Constructor that takes a camera reference
InputManager::InputManager(Camera* _camRef)
{
    m_deltaTime = 0;
    m_camera = _camRef;
    m_instance = this;
}

// Key callback function
void InputManager::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    if (_key == GLFW_KEY_C && _action == GLFW_PRESS)
    {
        static bool cursorVisible = true;
        cursorVisible = !cursorVisible;
        glfwSetInputMode(_window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    if (_key == GLFW_KEY_F && _action == GLFW_PRESS)
    {
        static bool wireframe = false;
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    if (_key == GLFW_KEY_P && _action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(_window, &xpos, &ypos);
        std::cout << "Cursor Position: (" << xpos << ", " << ypos << ")" << std::endl;
    }

    if (_key == GLFW_KEY_R && _action == GLFW_PRESS)
    {
        autoRotate = !autoRotate;
    }
}

// Mouse callback function
void InputManager::MouseCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    static bool firstMouse = true;
    static float lastX = 400, lastY = 400;
    if (firstMouse)
    {
        lastX = _xpos;
        lastY = _ypos;
        firstMouse = false;
    }

    float xoffset = _xpos - lastX;
    float yoffset = lastY - _ypos; // reversed since y-coordinates go from bottom to top
    lastX = _xpos;
    lastY = _ypos;

    if (!autoRotate)
    {
        xoffset *= m_camera->m_sensitivity * m_deltaTime;
        yoffset *= m_camera->m_sensitivity * m_deltaTime;
        m_camera->m_yaw += xoffset;
        m_camera->m_pitch += yoffset;
    }

    // Constrain the pitch angle to prevent screen flipping
    if (m_camera->m_pitch > 89.0f)
    {
        m_camera->m_pitch = 89.0f;
    }
    if (m_camera->m_pitch < -89.0f)
    {
        m_camera->m_pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));
    front.y = sin(glm::radians(m_camera->m_pitch));
    front.z = sin(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));
    m_camera->m_orientation = glm::normalize(front);
}

// Processes input for the current frame
void InputManager::ProcessInput(GLFWwindow* _window)
{
    m_currentFrame = glfwGetTime();
    m_deltaTime = m_currentFrame - m_lastFrame;
    m_lastFrame = m_currentFrame;

    float adjustedSpeed = m_camera->m_speed;
    if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    {
        adjustedSpeed *= 2.0f; // Double the speed when shift is pressed
    }

    if (!autoRotate)
    {
        if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            m_camera->m_radius -= adjustedSpeed * float(m_deltaTime);
        }
        if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            m_camera->m_radius += adjustedSpeed * float(m_deltaTime);
        }

        // Orbit around the center with LEFT and RIGHT keys
        if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            m_camera->m_yaw -= adjustedSpeed * float(m_deltaTime);
        }
        if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            m_camera->m_yaw += adjustedSpeed * float(m_deltaTime);
        }
    }
    else
    {
        m_camera->m_yaw += adjustedSpeed * float(m_deltaTime);
    }

    // Update camera position based on spherical coordinates
    m_camera->m_position.x = m_camera->m_radius * cos(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));
    m_camera->m_position.y = m_camera->m_radius * sin(glm::radians(m_camera->m_pitch));
    m_camera->m_position.z = m_camera->m_radius * sin(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));

    // Ensure the camera is looking at the origin
    m_camera->m_orientation = glm::normalize(-m_camera->m_position);
}

// Sets the cursor position callback
void InputManager::SetCursorPosCallback(GLFWwindow* _window)
{
    glfwSetCursorPosCallback(_window, StaticMouseCallback);
}

// Sets the key callback
void InputManager::SetKeyCallback(GLFWwindow* _window)
{
    glfwSetKeyCallback(_window, StaticKeyCallback);
}

// Static mouse callback function
void InputManager::StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    if (m_instance)
    {
        m_instance->MouseCallback(_window, _xpos, _ypos);
    }
}

// Static key callback function
void InputManager::StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (m_instance)
    {
        m_instance->KeyCallback(window, key, scancode, action, mods);
    }
}
