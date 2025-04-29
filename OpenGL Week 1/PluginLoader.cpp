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
        Debug::LogError("Failed to load [" + _dllPath + "] Error " + std::to_string(err));
        return false;
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
