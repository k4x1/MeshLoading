#include "InputManager.h"
InputManager* InputManager::m_instance = nullptr;

InputManager::InputManager(Camera* _camRef, Scene* _scene)
{
    m_camera = _camRef;
    m_scene = _scene;
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
    if (_key == GLFW_KEY_1 && _action == GLFW_PRESS) {
        currentScene = SceneType::Game;
        sceneChanged = true;
    }
    if (_key == GLFW_KEY_2 && _action == GLFW_PRESS) {
        currentScene = SceneType::HeightMap;
        sceneChanged = true;
    }
    if (_key == GLFW_KEY_3 && _action == GLFW_PRESS) {
        currentScene = SceneType::Noise;
        sceneChanged = true;
    }    
    if (_key == GLFW_KEY_4 && _action == GLFW_PRESS) {
        currentScene = SceneType::FrameBuffer;
        sceneChanged = true;
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

    xoffset *= m_camera->m_sensitivity * m_deltaTime;
    yoffset *= m_camera->m_sensitivity * m_deltaTime;

    m_camera->m_yaw += xoffset;
    m_camera->m_pitch += yoffset;

    // Constrain the pitch angle to prevent screen flipping
    if (m_camera->m_pitch > 89.0f)
        m_camera->m_pitch = 89.0f;
    if (m_camera->m_pitch < -89.0f)
        m_camera->m_pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));
    front.y = sin(glm::radians(m_camera->m_pitch));
    front.z = sin(glm::radians(m_camera->m_yaw)) * cos(glm::radians(m_camera->m_pitch));
    m_camera->m_orientation = glm::normalize(front);
}

void InputManager::ProcessInput(GLFWwindow* _window)
{
    
    m_currentFrame = glfwGetTime();
    m_deltaTime = m_currentFrame - m_lastFrame;
    m_lastFrame = m_currentFrame;
    
    if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_camera->m_position += (m_camera->m_speed * float(m_deltaTime)) * m_camera->m_up;
    }
    if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
        m_camera->m_position -= (m_camera->m_speed * float(m_deltaTime)) * m_camera->m_up;
    }
    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {

        m_camera->m_position += (m_camera->m_speed * float(m_deltaTime)) * m_camera->m_orientation;
    }
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
        m_camera->m_position -= glm::normalize(glm::cross(m_camera->m_orientation, m_camera->m_up)) * (m_camera->m_speed * float(m_deltaTime));
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
        m_camera->m_position += glm::normalize(glm::cross(m_camera->m_orientation, m_camera->m_up)) * (m_camera->m_speed * float(m_deltaTime));
    }
    static bool prevSKey = false;
    bool currSKey = (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
    bool ctrlPressed = (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);

    if (currSKey )
    {
        if (ctrlPressed && m_scene != nullptr && !prevSKey)
        {
            m_scene->SaveToFile(m_scene->sceneName + ".json");
        }
        else if (!ctrlPressed)
        {
            m_camera->m_position -= (m_camera->m_speed * float(m_deltaTime)) * m_camera->m_orientation;
        }
   

    }     
    prevSKey = currSKey;
   
}

void InputManager::SetCursorPosCallback(GLFWwindow* _window)
{
    glfwSetCursorPosCallback(_window, StaticMouseCallback);
    glfwSetScrollCallback(_window, StaticScrollCallback);
    glfwSetKeyCallback(_window, StaticKeyCallback);
}

void InputManager::StaticScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset) {
    if (m_instance) {
        m_instance->ScrollCallback(_window, _xoffset, _yoffset);
    }
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
void InputManager::ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset) {
    m_camera->m_FOV -= (float)_yoffset;

    if (m_camera->m_FOV < 1.0f) {
        m_camera->m_FOV = 1.0f;
    }
    if (m_camera->m_FOV > 179.0f) {
        m_camera->m_FOV = 179.0f;
    }
}
