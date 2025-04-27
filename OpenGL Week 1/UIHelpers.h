#pragma once
#include <imgui.h>
#include "FrameBuffer.h"
#include "Scene.h"
#include "GameObject.h"
#include "ImGuizmo.h"


enum class GizmoMode { Translate, Rotate, Scale };
static GizmoMode currentGizmoMode = GizmoMode::Translate;
enum class EditorState { Play, Pause, Stop };

namespace UIHelpers {
    void InitializeUI();
    void ShowDockSpace();

    void DrawSceneViewWindow(FrameBuffer* editorFB,
        GameObject* editorCamera,
        Scene* scene,
        GameObject* selected,
        float deltaTime);

    void DrawGameViewWindow(FrameBuffer* gameFB,
        GameObject* gameCam,
        Scene* scene,
        EditorState& state,
        float deltaTime);
    void DrawInspectorWindow(GameObject*& selected);
    void DrawHierarchyWindow(Scene* scene, GameObject*& selected, GameObject* editorCamera);
    void DrawProjectWindow();
    void DrawDebugWindow(bool* p_open = nullptr);
    void ShowGameObjectNode(GameObject* obj, GameObject*& selected, Scene* scene);
    extern bool g_SceneViewHovered;
    extern bool g_GameViewHovered;
}
