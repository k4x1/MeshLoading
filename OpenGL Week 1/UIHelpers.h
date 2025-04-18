#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "FrameBuffer.h"
#include "Camera.h"

namespace UIHelpers {
    // Call once at startup to load assets if needed
    void InitializeUI();

    // Dockspace & main menu bar
    void ShowDockSpace();

    // Individual windows
    void DrawSceneViewWindow(FrameBuffer* editorFB, Camera* editorCam);
    void DrawGameViewWindow(FrameBuffer* gameFB, Camera* gameCam, int& editorState);
    void DrawInspectorWindow(GameObject*& selected);
    void DrawHierarchyWindow(Scene* scene, GameObject*& selected);
    void DrawProjectWindow();
    void DrawConsoleWindow();

    // Helper for tree nodes
    void ShowGameObjectNode(GameObject* gameObject, GameObject*& selected);
}

