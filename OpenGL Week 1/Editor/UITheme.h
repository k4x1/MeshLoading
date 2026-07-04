#pragma once

#include <string>

class UITheme
{
public:
    static void ApplyDefaultTheme();
    static bool BeginPanelSection(const std::string& label);
    static void EndPanelSection();
};