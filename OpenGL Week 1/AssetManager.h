// AssetManager.h
#pragma once
#include <string>
#include <vector>
#include <filesystem>    
namespace fs = std::filesystem;

enum class AssetType { Texture, Model, Script, Prefab };

struct Asset {
    AssetType    type;
    std::string  name;
    std::string  path;
};

class AssetManager {
public:
    // Scan 'rootDir' (e.g. "Assets/") for known asset extensions
    static void LoadAssets(const std::string& rootDir);
    static const std::vector<Asset>& GetAssets();

private:
    static std::vector<Asset> assets;
};
