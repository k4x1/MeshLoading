#include "InspectorPreview.h"

#include <imgui.h>
#include <algorithm>
#include <cstdint>

void InspectorPreview::DrawPreview(
    const std::string& label,
    IInspectorPreviewRenderer& previewRenderer,
    EditorContext& context,
    float requestedSize,
    float minSize,
    float maxSize
)
{
    if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen) == false)
    {
        return;
    }

    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    float previewSize = requestedSize;

    if (previewSize <= 0.0f)
    {
        previewSize = availableSize.x;
    }

    previewSize = std::max(previewSize, minSize);

    previewSize = std::min(previewSize, maxSize);


    int width = static_cast<int>(previewSize);
    int height = static_cast<int>(previewSize);

    GLuint textureId = previewRenderer.RenderPreview(
        context,
        width,
        height
    );

    if (textureId == 0)
    {
        ImGui::TextUnformatted("Preview unavailable.");
        return;
    }

    float cursorX = ImGui::GetCursorPosX();
    float centeredX = cursorX + (availableSize.x - previewSize) * 0.5f;

    if (centeredX > cursorX)
    {
        ImGui::SetCursorPosX(centeredX);
    }

    ImGui::Image(
        static_cast<ImTextureID>(static_cast<intptr_t>(textureId)),
        ImVec2(previewSize, previewSize),
        ImVec2(0.0f, 1.0f),
        ImVec2(1.0f, 0.0f)
    );
}