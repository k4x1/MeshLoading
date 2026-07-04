#include "UITheme.h"

#include <imgui.h>

void UITheme::ApplyDefaultTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(7.0f, 4.0f);
    style.CellPadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 5.0f);
    style.ItemInnerSpacing = ImVec2(5.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;

    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    style.WindowRounding = 3.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 2.0f;
    style.LogSliderDeadzone = 4.0f;
    style.TabRounding = 2.0f;

    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ColorButtonPosition = ImGuiDir_Right;

    ImVec4 seafoam = ImVec4(0.28f, 0.82f, 0.70f, 1.00f);
    ImVec4 seafoamDark = ImVec4(0.12f, 0.45f, 0.40f, 1.00f);
    ImVec4 seafoamSoft = ImVec4(0.20f, 0.62f, 0.54f, 1.00f);
    ImVec4 seafoamDim = ImVec4(0.12f, 0.34f, 0.31f, 1.00f);

    ImVec4 panelDark = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
    ImVec4 panelMid = ImVec4(0.21f, 0.22f, 0.23f, 1.00f);
    ImVec4 panelLight = ImVec4(0.26f, 0.27f, 0.28f, 1.00f);
    ImVec4 panelHover = ImVec4(0.30f, 0.33f, 0.33f, 1.00f);
    ImVec4 panelActive = ImVec4(0.34f, 0.39f, 0.38f, 1.00f);

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.50f, 0.50f, 1.00f);

    colors[ImGuiCol_WindowBg] = panelDark;
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 0.98f);

    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg] = panelMid;
    colors[ImGuiCol_FrameBgHovered] = panelHover;
    colors[ImGuiCol_FrameBgActive] = panelActive;

    colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.23f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.12f, 0.13f, 1.00f);

    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.15f, 0.16f, 1.00f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.13f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.33f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = seafoamSoft;

    colors[ImGuiCol_CheckMark] = seafoam;
    colors[ImGuiCol_SliderGrab] = seafoamSoft;
    colors[ImGuiCol_SliderGrabActive] = seafoam;

    colors[ImGuiCol_Button] = panelLight;
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.43f, 0.41f, 1.00f);
    colors[ImGuiCol_ButtonActive] = seafoamDark;

    colors[ImGuiCol_Header] = ImVec4(0.23f, 0.25f, 0.26f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.42f, 0.39f, 1.00f);
    colors[ImGuiCol_HeaderActive] = seafoamDark;

    colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = seafoamSoft;
    colors[ImGuiCol_SeparatorActive] = seafoam;

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.82f, 0.70f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.28f, 0.82f, 0.70f, 0.55f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.28f, 0.82f, 0.70f, 0.85f);

    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.46f, 0.41f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.31f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.20f, 0.20f, 1.00f);

    colors[ImGuiCol_DockingPreview] = ImVec4(0.28f, 0.82f, 0.70f, 0.55f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.11f, 0.12f, 0.13f, 1.00f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.82f, 0.70f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.28f, 0.82f, 0.70f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.82f, 0.70f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.28f, 0.82f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.45f);

    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.035f);

    colors[ImGuiCol_PlotLines] = seafoamSoft;
    colors[ImGuiCol_PlotLinesHovered] = seafoam;
    colors[ImGuiCol_PlotHistogram] = seafoamSoft;
    colors[ImGuiCol_PlotHistogramHovered] = seafoam;
}