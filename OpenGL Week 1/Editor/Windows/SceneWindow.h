#pragma once
#include "../../EnginePluginAPI.h"
#include "../IEditorWindow.h"
#include "../EditorUI.h"

class CameraMovement;

class ENGINE_API SceneWindow : public IEditorWindow
{
public:
    void Draw(EditorContext& context) override;
    const char* GetWindowName() const override;

    bool IsHovered() const;
private:
    static AspectRatio   g_SceneAspect;
    bool isHovered = false;
    
    GameObject* cashedEditorCamera = nullptr;
    CameraMovement* editorCameraMovement = nullptr;
    
    void RefreshCameraMovement(GameObject* editorCamera);
};
