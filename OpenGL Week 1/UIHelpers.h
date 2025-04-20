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

    // Note: EditorState used here
    void DrawSceneViewWindow(FrameBuffer* editorFB,
        GameObject* editorCamera,
        Scene* scene,
        GameObject* selected,
        float deltaTime);

    void DrawGameViewWindow(FrameBuffer* gameFB,
        GameObject* editorCamera,
        Scene* scene,
        EditorState& state,
        float deltaTime);

    void DrawInspectorWindow(GameObject*& selected);
    void DrawHierarchyWindow(Scene* scene, GameObject*& selected);
    void DrawProjectWindow();
    void DrawDebugWindow(bool* p_open = nullptr);
}
