﻿#pragma once
#include <imgui.h>
#include "FrameBuffer.h"
#include "Scene.h"
#include "GameObject.h"
#include "ImGuizmo.h"


enum class ENGINE_API GizmoMode { Translate, Rotate, Scale };
static GizmoMode currentGizmoMode = GizmoMode::Translate;
enum class ENGINE_API EditorState { Play, Pause, Stop };
enum class ENGINE_API AspectRatio { Free = 0, R16_9, R4_3, R1_1 };
class ENGINE_API UIHelpers {
public:
    static void Init(GLFWwindow* window, const char* glsl_version = "#version 460");
    static void NewFrame();
    static void Render();
    static void Shutdown();
  
    static void InitializeUI();
    static void ShowDockSpace();

    static void DrawSceneViewWindow(FrameBuffer* editorFB,
        GameObject* editorCamera,
        Scene* scene,
        GameObject* selected,
        float deltaTime);

    static void DrawGameViewWindow(FrameBuffer* gameFB,
        Scene* scene,
        EditorState& state);
    static void DrawInspectorWindow(GameObject*& selected);
    static void DrawHierarchyWindow(Scene* scene, GameObject*& selected, GameObject* editorCamera);
    static void DrawProjectWindow();
    static void DrawDebugWindow(bool* p_open = nullptr);
    static void ShowGameObjectNode(GameObject* obj, GameObject*& selected, Scene* scene);
    static bool g_SceneViewHovered;
    static bool g_GameViewHovered; 
    static AspectRatio   g_SceneAspect;
    static AspectRatio   g_GameAspect;
    static bool g_LockMouse;
};
