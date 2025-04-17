#include "AssetManager.h"
#include <filesystem>

std::vector<Asset> AssetManager::assets;

void AssetManager::LoadAssets(const std::string& rootDir) {
    assets.clear();
    for (auto& p : std::filesystem::recursive_directory_iterator(rootDir)) {
        if (!p.is_regular_file()) continue;
        std::string ext = p.path().extension().string();
        AssetType type;
        if (ext == ".png" || ext == ".jpg")         type = AssetType::Texture;
        else if (ext == ".obj" || ext == ".fbx")    type = AssetType::Model;
        else if (ext == ".lua" || ext == ".cs")    type = AssetType::Script;
        else if (ext == ".prefab")                    type = AssetType::Prefab;
        else continue;
        assets.push_back({ type, p.path().filename().string(), p.path().string() });
    }
}

const std::vector<Asset>& AssetManager::GetAssets() {
    return assets;
}
