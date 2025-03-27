



#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>
#include "Camera.h"
#include "Scene.h"
class Scene;
class InputManager
{
private:
    double m_currentFrame = 0; // Current frame time
    double m_lastFrame = m_currentFrame; // Last frame time
    double m_deltaTime; // Time difference between frames
    Camera* m_camera; // Pointer to the camera object
    Scene* m_scene; 
    static InputManager* m_instance; // Singleton instance

    // Static callback functions for GLFW
    static void StaticMouseCallback(GLFWwindow* _window, double _xpos, double _ypos);
    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void StaticScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    bool autoRotate = false; // Flag for automatic rotation

public:
    bool m_updateLight = false;
    float m_spotlight = 1;
    float m_pointlight = 1;
    float m_dirlight = 1;
    enum class SceneType {
        Game,
        HeightMap,
        Noise,
        FrameBuffer
    };
    SceneType currentScene = SceneType::Game;
    bool sceneChanged = false;

    // Constructor that takes a camera reference
    InputManager(Camera* _camRef, Scene* _scene = nullptr);

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
