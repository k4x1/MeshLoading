#include "MaterialManager.h"

#include "../Utils/Debug.h"

std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::materials;

Material* MaterialManager::GetMaterial(const std::string& path)
{
    std::unordered_map<std::string, std::unique_ptr<Material>>::iterator iterator =
        materials.find(path);

    if (iterator != materials.end())
    {
        return iterator->second.get();
    }

    std::unique_ptr<Material> material = std::make_unique<Material>();

    if (material->LoadFromFile(path) == false)
    {
        DEBUG_ERR("Failed to load material: " << path);
        return nullptr;
    }

    Material* materialPointer = material.get();
    materials[path] = std::move(material);

    return materialPointer;
}

void MaterialManager::ReloadMaterial(const std::string& path)
{
    materials.erase(path);
    GetMaterial(path);
}