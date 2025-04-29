#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif 
struct GLFWwindow;
namespace Engine {
	ENGINE_API bool   InitGLFW();
	ENGINE_API void   TerminateGLFW();
	ENGINE_API void   SetGLFWWindowHints(int major, int minor, int samples);
	ENGINE_API GLFWwindow* CreateGLFWWindow(int w, int h, const char* title);
	ENGINE_API void   MakeContextCurrent(GLFWwindow* window);
	ENGINE_API void   PollEvents();
	ENGINE_API void   SwapBuffers(GLFWwindow* window);
	ENGINE_API bool   WindowShouldClose(GLFWwindow* window);
	ENGINE_API double GetTime();
}