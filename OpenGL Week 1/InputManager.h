#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glew.h>     
#include <unordered_map>
#include <glfw3.h>

class InputManager {
public:
  

    static InputManager& Instance();

    void Update();

    bool GetKey(int key) const;
    bool GetKeyDown(int key) const;
    bool GetKeyUp(int key) const;

    double GetMouseX() const;
    double GetMouseY() const;
    void GetMousePosition(double& x, double& y) const;

    double GetScrollOffset() const;

    static void SetCallbacks(GLFWwindow* window);
    GLFWwindow* GetWindow() const { return m_window; }
   // SceneType currentScene = SceneType::Game;
    bool sceneChanged = false;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

private:
    InputManager();

    std::unordered_map<int, bool> currentKeyStates;
    std::unordered_map<int, bool> previousKeyStates;
   static GLFWwindow* m_window;
    double mouseX = 0.0, mouseY = 0.0;
    double scrollOffset = 0.0;

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif 
