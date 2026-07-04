#include "DebugWindow.h"
#include <imgui.h>
#include "../../Utils/Debug.h"
#include "../EditorWindowRegistry.h"

REGISTER_EDITOR_WINDOW(DebugWindow);

void DebugWindow::Draw(EditorContext& context)
{
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::Button("Clear")) {
        Debug::ClearEntries();
    }
    ImGui::SameLine();
    static bool autoScroll = true;
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::Separator();
    ImGui::BeginChild("##DebugScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    const auto& entries = Debug::GetEntries();
    for (const auto& e : entries) {
        ImVec4 col;
        switch (e.level) {
        case DebugLevel::Log:       col = ImVec4(1, 1, 1, 1);    break; // white
        case DebugLevel::Warning:   col = ImVec4(1, 1, 0, 1);    break; // yellow
        case DebugLevel::Error:     col = ImVec4(1, 0, 0, 1);    break; // red
        case DebugLevel::Exception: col = ImVec4(1, 0.5f, 0, 1); break; // orange
        }
        ImGui::PushStyleColor(ImGuiCol_Text, col);

        const char* lvlName = "UNK";
        switch (e.level) {
        case DebugLevel::Log:       lvlName = "LOG";  break;
        case DebugLevel::Warning:   lvlName = "WARN"; break;
        case DebugLevel::Error:     lvlName = "ERR";  break;
        case DebugLevel::Exception: lvlName = "EXPT"; break;
        }

        ImGui::Text("[%s] [%s] %s:%d  %s",
            e.timestamp.c_str(),
            lvlName,
            e.file.c_str(),
            e.line,
            e.message.c_str()
        );

        ImGui::PopStyleColor();
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

const char* DebugWindow::GetWindowName() const
{
    return "Debug";
}
