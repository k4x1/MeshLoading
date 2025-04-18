#pragma once

#include "Component.h"
#include <string>
#include <memory>

/// Simple placeholder for file‑based scripts.
/// You can swap in your own engine (Lua, JS, etc.) later.
class ScriptComponent : public Component {
public:
    /// @param scriptPath  Path to your script file (e.g. Lua, JS, or C# stub)
    explicit ScriptComponent(const std::string& scriptPath);

    /// Called once at scene start
    void Start() override;

    /// Called every frame
    void Update(float dt) override;

    /// Draw UI in the inspector
    void OnInspectorGUI() override;

private:
    std::string      m_scriptPath;
    bool             m_loaded = false;
    // e.g. a handle/pointer to your script VM or AST
    // std::unique_ptr<YourScriptVM> m_vm;  
};
