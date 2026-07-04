#pragma once
#include "../../EnginePluginAPI.h"
#include "../IEditorWindow.h"
#include "../EditorUI.h"

class CameraMovement;

class ENGINE_API GameWindow : public IEditorWindow
{
public:
    void Draw(EditorContext& context) override;
    const char* GetWindowName() const override;

    bool IsHovered() const;

private:
    AspectRatio gameAspect = AspectRatio::R16_9;
    bool lockMouse = false;
    bool isHovered = false;

    GameObject* cashedEditorCamera = nullptr;
    CameraMovement* editorCameraMovement = nullptr;
    
    void RefreshCameraMovement(GameObject* editorCamera);
};
