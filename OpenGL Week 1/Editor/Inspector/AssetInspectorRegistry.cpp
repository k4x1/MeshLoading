#include "AssetInspectorRegistry.h"

#include "../../Utils/Debug.h"

#include <vector>

struct AssetInspectorRegistryEntry
{
    std::string inspectorName;
    AssetInspectorFactory factory = nullptr;
    std::unique_ptr<IAssetInspector> inspector = nullptr;
};

static std::vector<AssetInspectorRegistryEntry>& GetAssetInspectorRegistryEntries()
{
    static std::vector<AssetInspectorRegistryEntry> entries;
    return entries;
}
void AssetInspectorRegistry::RegisterInspector(
    const std::string& inspectorName,
    AssetInspectorFactory factory
)
{
    std::vector<AssetInspectorRegistryEntry>& entries = GetAssetInspectorRegistryEntries();

    for (const AssetInspectorRegistryEntry& entry : entries)
    {
        if (entry.inspectorName == inspectorName)
        {
            return;
        }
    }

    AssetInspectorRegistryEntry entry;
    entry.inspectorName = inspectorName;
    entry.factory = factory;
    entry.inspector = nullptr;

    entries.push_back(std::move(entry));
}
IAssetInspector* AssetInspectorRegistry::GetInspectorForAsset(const Asset& asset)
{
    std::vector<AssetInspectorRegistryEntry>& entries = GetAssetInspectorRegistryEntries();

    for (AssetInspectorRegistryEntry& entry : entries)
    {
        if (entry.factory == nullptr)
        {
            continue;
        }

        if (entry.inspector == nullptr)
        {
            entry.inspector = entry.factory();
        }

        if (entry.inspector == nullptr)
        {
            continue;
        }

        if (entry.inspector->CanInspect(asset))
        {
            return entry.inspector.get();
        }
    }

    return nullptr;
}
bool AssetInspectorRegistry::DrawInspectorForAsset(
    EditorContext& context,
    const Asset& asset
)
{
    IAssetInspector* inspector = GetInspectorForAsset(asset);

    if (inspector == nullptr)
    {
        return false;
    }

    inspector->Draw(context, asset);
    return true;
}