#include "AssetManager.h"
#include "AssetTemplates.h"
#include <fstream>
#include <cctype>
#include <unordered_set>

#include "Utils/Debug.h"

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
            else if (ext == ".mat" || ext == ".material")
                type = AssetType::Material;
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
        DEBUG_ERR("AssetManager filesystem error: " << e.what());
    }
}

const std::vector<Asset>& AssetManager::GetAssets() {
    return assets;
}

bool AssetManager::IsHiddenExtension(const fs::path& filePath)
{
    static const std::unordered_set<std::string> extensions =
    {
    ".mtl",
    ".meta",
    ".tmp",
    ".cache",
    ".pdb",
    ".ilk",
    ".obj.user"
    };

    std::string extension = filePath.extension().string();
    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(std::tolower(character));
        }
    );
    return extensions.find(extension) != extensions.end();
}

bool AssetManager::CreateAsset(
    AssetType assetType,
    const fs::path& directoryPath,
    const std::string& assetName
)
{
    if (assetName.empty())
    {
        DEBUG_ERR("Cannot create asset with empty name.");
        // update to delete the creation 
        return false;
    }

    if (fs::exists(directoryPath) == false)
    {
        DEBUG_ERR("Target directory does not exist: " << directoryPath.string());
        return false;
    }

    if (assetType == AssetType::Material)
    {
        fs::path assetBasePath = GetUniqueAssetPath(directoryPath, assetName, ".mat");

        fs::path materialPath = assetBasePath;
        materialPath += ".mat";

        std::ofstream materialFile(materialPath);

        if (materialFile.is_open() == false)
        {
            DEBUG_ERR("Failed to create material: " << materialPath.string());
            return false;
        }

        materialFile << AssetTemplates::GetMaterialTemplate();
        materialFile.close();

        DEBUG_LOG("Created material: " << materialPath.string());

        LoadAssets("Assets");
        return true;
    }

    if (assetType == AssetType::Script)
    {
        std::string className;

        for (char character : assetName)
        {
            if (std::isalnum(static_cast<unsigned char>(character)) || character == '_')
            {
                className += character;
            }
        }

        if (className.empty())
        {
            DEBUG_ERR("Script name does not contain any valid C++ class characters.");
            return false;
        }

        if (std::isdigit(static_cast<unsigned char>(className[0])))
        {
            className = "Component" + className;
        }

        fs::path assetBasePath = GetUniqueAssetPath(directoryPath, className, ".h");
        className = assetBasePath.filename().string();

        fs::path headerPath = assetBasePath;
        headerPath += ".h";

        fs::path sourcePath = assetBasePath;
        sourcePath += ".cpp";

        std::ofstream headerFile(headerPath);

        if (headerFile.is_open() == false)
        {
            DEBUG_ERR("Failed to create script header: " << headerPath.string());
            return false;
        }

        headerFile << AssetTemplates::GetScriptHeaderTemplate(className);
        headerFile.close();

        std::ofstream sourceFile(sourcePath);

        if (sourceFile.is_open() == false)
        {
            DEBUG_ERR("Failed to create script source: " << sourcePath.string());

            std::error_code errorCode;
            fs::remove(headerPath, errorCode);

            if (errorCode)
            {
                DEBUG_WARN("Failed to clean up header after source creation failed: "
                    << headerPath.string());
            }

            return false;
        }

        sourceFile << AssetTemplates::GetScriptSourceTemplate(className);
        sourceFile.close();

        DEBUG_LOG("Created script: " << headerPath.string() << " and " << sourcePath.string());

        LoadAssets("Assets");
        return true;
    }

    DEBUG_WARN("Asset type cannot be created from editor yet.");
    return false;
}
bool AssetManager::CreateFolder(
    const fs::path& directoryPath,
    const std::string& folderName
)
{
    if (folderName.empty())
    {
        DEBUG_ERR("Cannot create folder with empty name.");
        return false;
    }

    if (fs::exists(directoryPath) == false)
    {
        DEBUG_ERR("arget directory does not exist: " << directoryPath.string());
        return false;
    }

    fs::path folderPath = directoryPath / folderName;

    int copyIndex = 1;

    while (fs::exists(folderPath))
    {
        folderPath = directoryPath / (folderName + std::to_string(copyIndex));
        copyIndex++;
    }

    std::error_code errorCode;
    fs::create_directory(folderPath, errorCode);

    if (errorCode)
    {
        DEBUG_ERR("Failed to create folder: "
            << folderPath.string()
            << " "
            << errorCode.message());

        return false;
    }

    DEBUG_LOG("Created folder: " << folderPath.string());

    LoadAssets("Assets");
    return true;
}
fs::path AssetManager::GetUniqueAssetPath(
    const fs::path& directoryPath,
    const std::string& baseName,
    const std::string& extensionToCheck
)
{
    fs::path assetBasePath = directoryPath / baseName;

    fs::path assetPathToCheck = assetBasePath;
    assetPathToCheck += extensionToCheck;

    int copyIndex = 1;

    while (fs::exists(assetPathToCheck))
    {
        assetBasePath = directoryPath / (baseName + std::to_string(copyIndex));

        assetPathToCheck = assetBasePath;
        assetPathToCheck += extensionToCheck;

        copyIndex++;
    }

    return assetBasePath;
}
