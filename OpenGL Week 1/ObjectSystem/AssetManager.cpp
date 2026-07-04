#include "AssetManager.h"
#include <iostream>

std::vector<Asset> AssetManager::assets;

void AssetManager::LoadAssets(const std::string& rootDir) {
    assets.clear();
    try {
        for (auto const& entry : fs::recursive_directory_iterator(rootDir)) {
            if (!fs::is_regular_file(entry.path()))
                continue;

            auto ext = entry.path().extension().string();
            AssetType type;
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
                type = AssetType::Texture;
            else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf")
                type = AssetType::Model;
            else if (ext == ".h" || ext == ".cpp")
                type = AssetType::Script;
            else if (ext == ".prefab" || ext == ".json")
                type = AssetType::Prefab;
            else
                continue;

            assets.push_back({
                type,
                entry.path().filename().string(),
                entry.path().string()
                });
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "AssetManager filesystem error: " << e.what() << "\n";
    }
}

const std::vector<Asset>& AssetManager::GetAssets() {
    return assets;
}
