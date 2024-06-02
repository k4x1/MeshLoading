/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : InputManager.h
Description : Header file for the InputManager class, which handles keyboard and mouse input for the application.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>
#include "Camera.h"

// Manages input handling for the application.
class InputManager
{
private:
    double m_currentFrame = 0; // Current frame time
    double m_lastFrame = m_currentFrame; // Last frame time
    double m_deltaTime; // Time difference between frames
    Camera* m_camera; // Pointer to the camera object
    static InputManager* m_instance; // Singleton instance

    // Static callback functions for GLFW
    static void StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos);
    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void StaticScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    bool autoRotate = false; // Flag for automatic rotation

public:
    // Constructor that takes a camera reference
    InputManager(Camera* _camRef);

    // Key callback function
    void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);

    // Mouse callback function
    void MouseCallback(GLFWwindow* _window, double _xpos, double _ypos);

    // Scroll callback function
    void ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);

    // Processes input for the current frame
    void ProcessInput(GLFWwindow* _window);

    // Sets the cursor position callback
    static void SetCursorPosCallback(GLFWwindow* _window);

    // Sets the key callback
    static void SetKeyCallback(GLFWwindow* _window);
};

#endif
