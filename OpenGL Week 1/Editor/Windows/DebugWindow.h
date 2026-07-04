#pragma once
#include "../../EnginePluginAPI.h"
#include "../IEditorWindow.h"

class ENGINE_API DebugWindow : public IEditorWindow
{
public:
    void Draw(EditorContext& context) override;
    const char* GetWindowName() const override;
};

