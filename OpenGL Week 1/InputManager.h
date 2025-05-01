#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glew.h>     
#include <unordered_map>
#include <glm.hpp>
#include <glfw3.h>
#include "EnginePluginAPI.h"

class ENGINE_API InputManager {
public:
  

    static InputManager& Instance();

    void Update();

    bool GetKey(int key) const;
    bool GetKeyDown(int key) const;
    bool GetKeyUp(int key) const;

    double GetMouseX() const;
    double GetMouseY() const;
    void GetMousePosition(double& x, double& y) const;
    glm::vec2 GetMouseDelta() const;

    bool GetMouseButton(int button) const;
    bool GetMouseButtonDown(int button) const;
    bool GetMouseButtonUp(int button) const;

    double GetScrollOffset() const;

    void SetCallbacks(GLFWwindow* window);
    GLFWwindow* GetWindow() const { return m_window; }
    bool sceneChanged = false;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

private:
    InputManager();

    std::unordered_map<int, bool> currentKeyStates;
    std::unordered_map<int, bool> previousKeyStates;
    std::unordered_map<int, bool> currentButtonStates;
    std::unordered_map<int, bool> previousButtonStates;


    GLFWwindow* m_window = nullptr;
    double mouseX = 0.0, mouseY = 0.0;
    double previousMouseX = 0.0, previousMouseY = 0.0;
    double scrollOffset = 0.0;

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

};

#endif 
