#pragma once

#include "../../EnginePluginAPI.h"

#include <glew.h>

struct EditorContext;

class ENGINE_API IInspectorPreviewRenderer
{
public:
    virtual ~IInspectorPreviewRenderer() = default;

    virtual GLuint RenderPreview(
        EditorContext& context,
        int width,
        int height
    ) = 0;
};