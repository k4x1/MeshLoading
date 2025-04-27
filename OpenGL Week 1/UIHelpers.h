#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "FrameBuffer.h"
#include "Scene.h"
#include "GameObject.h"
#include "ImGuizmo.h"


enum class ENGINE_API GizmoMode { Translate, Rotate, Scale };
static GizmoMode currentGizmoMode = GizmoMode::Translate;
enum class ENGINE_API EditorState { Play, Pause, Stop };

class ENGINE_API UIHelpers {
public:
    static void Init(GLFWwindow*& window, const char* glsl_version = "#version 130");
    static void NewFrame();
    static void Render();
    static void Shutdown();
    static ImGuiContext* GetImGuiContext();
    static void SetImGuiContext(ImGuiContext* context);

    static void InitializeUI();
    static void ShowDockSpace();

    static void DrawSceneViewWindow(FrameBuffer* editorFB,
        GameObject* editorCamera,
        Scene* scene,
        GameObject* selected,
        float deltaTime);

    static void DrawGameViewWindow(FrameBuffer* gameFB,
        GameObject* gameCam,
        Scene* scene,
        EditorState& state,
        float deltaTime);
    static void DrawInspectorWindow(GameObject*& selected);
    static void DrawHierarchyWindow(Scene* scene, GameObject*& selected, GameObject* editorCamera);
    static void DrawProjectWindow();
    static void DrawDebugWindow(bool* p_open = nullptr);
    static void ShowGameObjectNode(GameObject* obj, GameObject*& selected, Scene* scene);
    static bool g_SceneViewHovered;
    static bool g_GameViewHovered;
};
