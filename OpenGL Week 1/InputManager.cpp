#include "InputManager.h"
InputManager* InputManager::instance = nullptr;

InputManager::InputManager(Camera* _camRef)
{
    m_camera = _camRef;
    instance = this;
}

void InputManager::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    if (_key == GLFW_KEY_C && _action == GLFW_PRESS)
    {
        static bool cursorVisible = true;
        cursorVisible = !cursorVisible;
        glfwSetInputMode(_window, GLFW_CURSOR, cursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    if (_key == GLFW_KEY_O && _action == GLFW_PRESS)
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
}

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

    xoffset *= m_camera->sensitivity * m_deltaTime;
    yoffset *= m_camera->sensitivity * m_deltaTime;

    m_camera->yaw += xoffset;
    m_camera->pitch += yoffset;

    // Constrain the pitch angle to prevent screen flipping
    if (m_camera->pitch > 89.0f)
        m_camera->pitch = 89.0f;
    if (m_camera->pitch < -89.0f)
        m_camera->pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(m_camera->yaw)) * cos(glm::radians(m_camera->pitch));
    front.y = sin(glm::radians(m_camera->pitch));
    front.z = sin(glm::radians(m_camera->yaw)) * cos(glm::radians(m_camera->pitch));
    m_camera->Orientation = glm::normalize(front);
}

void InputManager::ProcessInput(GLFWwindow* _window)
{
    m_currentFrame = glfwGetTime();
    m_deltaTime = m_currentFrame - m_lastFrame;
    m_lastFrame = m_currentFrame;
    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS){
        m_camera->Position += (m_camera->speed * float(m_deltaTime)) * m_camera->Orientation;
     
    }
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS){
        m_camera->Position -= (m_camera->speed * float(m_deltaTime)) * m_camera->Orientation;
    }

    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS){
        m_camera->Position -= glm::normalize(glm::cross(m_camera->Orientation, m_camera->Up)) * (m_camera->speed * float(m_deltaTime));
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS){
        m_camera->Position += glm::normalize(glm::cross(m_camera->Orientation, m_camera->Up)) * (m_camera->speed * float(m_deltaTime));
    }
    if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS){
        m_camera->Position += (m_camera->speed * float(m_deltaTime)) * m_camera->Up;
    }
    if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS){
        m_camera->Position -= (m_camera->speed * float(m_deltaTime)) * m_camera->Up;
    }
}

void InputManager::SetCursorPosCallback(GLFWwindow* _window)
{
    glfwSetCursorPosCallback(_window, StaticMouseCallback);
}

void InputManager::StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    if (instance)
    {
        instance->MouseCallback(_window, _xpos, _ypos);
    }
}