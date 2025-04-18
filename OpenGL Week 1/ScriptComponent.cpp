#include "ScriptComponent.h"
#include <iostream>
#include <imgui.h>

ScriptComponent::ScriptComponent(const std::string& scriptPath)
    : m_scriptPath(scriptPath)
{}

void ScriptComponent::Start() {
    // Placeholder: load or compile your script here
    // e.g. m_vm = std::make_unique<YourScriptVM>(m_scriptPath);
    std::cout << "[ScriptComponent] Loading script: " << m_scriptPath << "\n";
    m_loaded = true;
}

void ScriptComponent::Update(float dt) {
    if (!m_loaded) return;

    // Placeholder: call the script's update function, if any
    // e.g. m_vm->Call("update", dt);
    // For now, just demonstrate:
    // std::cout << "[ScriptComponent] Update(" << dt << ")\n";
}

void ScriptComponent::OnInspectorGUI() {
    ImGui::TextWrapped("Script: %s", m_scriptPath.c_str());
    if (ImGui::Button("Reload Script")) {
        // Placeholder: re‑load/refresh your script on demand
        std::cout << "[ScriptComponent] Reloading: " << m_scriptPath << "\n";
        // e.g. m_vm->Reload(m_scriptPath);
    }
}
