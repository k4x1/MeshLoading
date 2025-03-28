#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "Camera.h"  // This is your Camera component (derived from Component)
#include "Scene.h"
#include "Component.h"
class Scene;

class InputManager {
private:
    double m_currentFrame = 0;          // Current frame time
    double m_lastFrame = m_currentFrame;  // Last frame time
    double m_deltaTime;                 // Time difference between frames
    Camera* m_camera;                   // Pointer to the Camera component
    Scene* m_scene;
    static InputManager* m_instance;    // Singleton instance

    // Static callback functions for GLFW
    static void StaticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void StaticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

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

    // Constructor takes a Camera pointer and an optional Scene pointer
    InputManager(Camera* _camRef, Scene* _scene = nullptr);

    // Key callback function
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Mouse callback function
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    // Scroll callback function
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // Processes input for the current frame
    void ProcessInput(GLFWwindow* window);

    // Sets the GLFW callback functions
    static void SetCursorPosCallback(GLFWwindow* window);
    static void SetKeyCallback(GLFWwindow* window);
};

#endif
