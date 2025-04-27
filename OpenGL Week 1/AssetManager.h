#pragma once
#include <string>
#include <vector>
#include <filesystem>    
#include "EnginePluginAPI.h"
namespace fs = std::filesystem;

enum class ENGINE_API AssetType { Texture, Model, Script, Prefab };

struct ENGINE_API Asset {
    AssetType    type = AssetType::Script;
    std::string  name = "amongus";
    std::string  path = "";
};

class ENGINE_API AssetManager {
public:
    static void LoadAssets(const std::string& rootDir);
    static const std::vector<Asset>& GetAssets();

private:
    static std::vector<Asset> assets;
};
