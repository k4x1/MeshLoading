#include "PluginLoader.h"


PluginLoader::PluginLoader(const std::string& dllpath)
{
    _dllPath = dllpath;
    _hModule = nullptr;
}

bool PluginLoader::Load()
{
    _hModule = ::LoadLibraryA(_dllPath.c_str());
    if (!_hModule) {
        auto err = GetLastError();
        
      
        DEBUG_ERR("Failed to load [" << _dllPath << "] Error " << std::to_string(err));
        return false;
    }
    using SetCtxFn = void(*)(ImGuiContext*);
    auto fn = (SetCtxFn)::GetProcAddress(_hModule, "SetImGuiContext");
    if (fn) {
        fn(ImGui::GetCurrentContext());
    }
    else {
        DEBUG_ERR("Plugin missing SetImGuiContext export");
    }
    return true;
}


void PluginLoader::Unload()
{
    if (_hModule)
    {
        ::FreeLibrary(_hModule);
        _hModule = nullptr;
    }
}
