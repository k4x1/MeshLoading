#pragma once
#include "../EnginePluginAPI.h"
#include "EditorContext.h"

class ENGINE_API IEditorWindow
{
public:
    virtual ~IEditorWindow() = default;
    virtual const char* GetWindowName() const = 0;
    virtual void Draw(EditorContext& context) = 0;
};