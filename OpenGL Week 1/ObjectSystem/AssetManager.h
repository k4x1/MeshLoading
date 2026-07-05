#pragma once
#include <string>
#include <vector>
#include <filesystem>    
#include "../EnginePluginAPI.h"
namespace fs = std::filesystem;

enum class ENGINE_API AssetType { Texture, Model, Script, Prefab, Material};

struct ENGINE_API Asset {
    AssetType    type = AssetType::Script;
    std::string  name = "amongus";
    std::string  path = "";
};

class ENGINE_API AssetManager {
public:
    static void LoadAssets(const std::string& rootDir);
    static const std::vector<Asset>& GetAssets();
    
    static bool IsHiddenExtension(const fs::path& filePath);
    
    static bool CreateAsset(
        AssetType assetType,
        const fs::path& directoryPath,
        const std::string& assetName
    );

    static bool CreateFolder(
        const fs::path& directoryPath,
        const std::string& folderName
    );
    static const Asset* FindAssetByPath(const std::string& assetPath);

private:
    static std::vector<Asset> assets;
    
    static fs::path GetUniqueAssetPath(
        const fs::path& directoryPath,
        const std::string& baseName,
        const std::string& extensionToCheck);
    
   
};
