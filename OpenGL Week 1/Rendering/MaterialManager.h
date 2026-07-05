#pragma once

#include "../EnginePluginAPI.h"
#include "Material.h"

#include <memory>
#include <string>
#include <unordered_map>

class ENGINE_API MaterialManager
{
public:
    static Material* GetMaterial(const std::string& path);
    static void ReloadMaterial(const std::string& path);

private:
    static std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};