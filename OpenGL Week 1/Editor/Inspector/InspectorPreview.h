#pragma once

#include "../../EnginePluginAPI.h"
#include "IInspectorPreviewRenderer.h"

#include <string>

class ENGINE_API InspectorPreview
{
public:
    static void DrawPreview(
        const std::string& label,
        IInspectorPreviewRenderer& previewRenderer,
        EditorContext& context,
        float requestedSize = 0.0f,
        float minSize = 64.0f,
        float maxSize = 512.0f
    );
};