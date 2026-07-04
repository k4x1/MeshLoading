#pragma once
#include "../Scene/Scene.h"
#include "../ObjectSystem/GameObject.h"


struct EditorContext;
class IEditorWindow;

enum class ENGINE_API GizmoMode { Translate, Rotate, Scale };
static GizmoMode currentGizmoMode = GizmoMode::Translate;
enum class ENGINE_API EditorState { Play, Pause, Stop };
enum class ENGINE_API AspectRatio { Free = 0, R16_9, R4_3, R1_1 };
class ENGINE_API EditorUI {
public:
    static void Init(GLFWwindow* window, const char* glsl_version = "#version 460");
    static void NewFrame();
    static void Render();
    static void Shutdown();
  
    static void InitializeUI();
    static void ShowDockSpace();
    
    static void DrawWindows(EditorContext& context);

    static void ShowGameObjectNode(GameObject* obj, GameObject*& selected, Scene* scene);

private:
    static std::vector<std::unique_ptr<IEditorWindow>> editorWindows;
};
