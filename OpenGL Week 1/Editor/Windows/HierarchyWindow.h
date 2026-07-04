#pragma once
#include "../../EnginePluginAPI.h"
#include "../../ObjectSystem/AssetManager.h"
#include "../IEditorWindow.h"
 
class ENGINE_API HierarchyWindow : public IEditorWindow
{
public:
    void Draw(EditorContext& context) override;
    const char* GetWindowName() const override;
};
