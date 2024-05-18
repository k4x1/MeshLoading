#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H


#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>

#include <iostream>

#include "Camera.h"

class InputManager
{
private:
    double m_currentFrame = 0;
    double m_lastFrame = m_currentFrame;
    double m_deltaTime;
    Camera* m_camera;
    static InputManager* m_instance;
    static void StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos);
    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    bool autoRotate = false; 

public:
    InputManager(Camera* _camRef);
    void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
    
    void MouseCallback(GLFWwindow* _window, double _xpos, double _ypos);
    void ProcessInput(GLFWwindow* _window);

    static void SetCursorPosCallback(GLFWwindow* _window);
    static void SetKeyCallback(GLFWwindow* _window);
};

#endif