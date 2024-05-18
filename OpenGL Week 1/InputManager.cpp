#include "InputManager.h"
InputManager* InputManager::m_instance = nullptr;

InputManager::InputManager(Camera* _camRef)
{
    m_camera = _camRef;
    m_instance = this;
}

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
    if(!autoRotate){
        xoffset *= m_camera->sensitivity * m_deltaTime;
        yoffset *= m_camera->sensitivity * m_deltaTime;
    }

    m_camera->yaw += xoffset;
    m_camera->pitch += yoffset;

    // Constrain the pitch angle to prevent screen flipping
    if (m_camera->pitch > 89.0f){
        m_camera->pitch = 89.0f;
    }
    if (m_camera->pitch < -89.0f){
        m_camera->pitch = -89.0f;
    }

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

    float adjustedSpeed = m_camera->speed;
    if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        adjustedSpeed *= 2.0f; // Double the speed when shift is pressed
    }
/*
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
        m_camera->Position += (adjustedSpeed * float(m_deltaTime)) * m_camera->Orientation;
    }
    if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        m_camera->Position -= (adjustedSpeed * float(m_deltaTime)) * m_camera->Orientation;
    }
    if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        m_camera->Position -= glm::normalize(glm::cross(m_camera->Orientation, m_camera->Up)) * (adjustedSpeed * float(m_deltaTime));
    }
    if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        m_camera->Position += glm::normalize(glm::cross(m_camera->Orientation, m_camera->Up)) * (adjustedSpeed * float(m_deltaTime));
    }
  */  

    // Adjust radius with UP and DOWN keys

    if(!autoRotate){
        if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
            m_camera->radius -= adjustedSpeed * float(m_deltaTime);
        }
        if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            m_camera->radius += adjustedSpeed * float(m_deltaTime);
        }

        // Orbit around the center with LEFT and RIGHT keys
        if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            m_camera->yaw -= adjustedSpeed * float(m_deltaTime);
        }
        if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            m_camera->yaw += adjustedSpeed * float(m_deltaTime);
        }
    }
    else {
        m_camera->yaw += adjustedSpeed * float(m_deltaTime);
    }
   
    // Update camera position based on spherical coordinates
    m_camera->Position.x = m_camera->radius * cos(glm::radians(m_camera->yaw)) * cos(glm::radians(m_camera->pitch));
    m_camera->Position.y = m_camera->radius * sin(glm::radians(m_camera->pitch));
    m_camera->Position.z = m_camera->radius * sin(glm::radians(m_camera->yaw)) * cos(glm::radians(m_camera->pitch));

    // Ensure the camera is looking at the origin
    m_camera->Orientation = glm::normalize(-m_camera->Position);
}

void InputManager::SetCursorPosCallback(GLFWwindow* _window)
{
    glfwSetCursorPosCallback(_window, StaticMouseCallback);
}
void InputManager::SetKeyCallback(GLFWwindow* _window)
{
    glfwSetKeyCallback(_window, StaticKeyCallback);
}

void InputManager::StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    if (m_instance)
    {
        m_instance->MouseCallback(_window, _xpos, _ypos);
    }
}
void InputManager::StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (m_instance)
    {
        m_instance->KeyCallback(window, key, scancode, action, mods);
    }
}
