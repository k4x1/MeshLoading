#define ENGINE_EXPORTS
#include "EnginePluginAPI.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine {
    bool InitGLFW() {
        return ::glfwInit() == GLFW_TRUE;
    }
    void TerminateGLFW() { ::glfwTerminate(); }
    void SetGLFWWindowHints(int major, int minor, int samples) {
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        ::glfwWindowHint(GLFW_SAMPLES, samples);
    }
    GLFWwindow* CreateGLFWWindow(int w, int h, const char* title) {
        return ::glfwCreateWindow(w, h, title, nullptr, nullptr);
    }
    void MakeContextCurrent(GLFWwindow* window) {
        ::glfwMakeContextCurrent(window);
    }
    void PollEvents() { ::glfwPollEvents(); }
    void SwapBuffers(GLFWwindow* w) { ::glfwSwapBuffers(w); }
    bool WindowShouldClose(GLFWwindow* w) {
        return ::glfwWindowShouldClose(w) == GLFW_TRUE;
    }
}
