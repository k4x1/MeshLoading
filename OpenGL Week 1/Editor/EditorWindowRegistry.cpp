#include "EditorWindowRegistry.h"
#include "../Utils/Debug.h"

#include <vector>

struct EditorWindowRegistryEntry
{
    std::string windowName;
    EditorWindowFactory factory = nullptr;
};

static std::vector<EditorWindowRegistryEntry>& GetEditorWindowRegistryEntries()
{
    static std::vector<EditorWindowRegistryEntry> entries;
    return entries;
}

void EditorWindowRegistry::RegisterWindow(
    const std::string& windowName,
    EditorWindowFactory factory
)
{
    std::vector<EditorWindowRegistryEntry>& entries = GetEditorWindowRegistryEntries();

    for (const EditorWindowRegistryEntry& entry : entries)
    {
        if (entry.windowName == windowName)
        {
            return;
        }
    }

    entries.push_back({ windowName, factory });

}

std::vector<std::unique_ptr<IEditorWindow>> EditorWindowRegistry::CreateWindows()
{
    std::vector<std::unique_ptr<IEditorWindow>> windows;

    std::vector<EditorWindowRegistryEntry>& entries = GetEditorWindowRegistryEntries();

    for (const EditorWindowRegistryEntry& entry : entries)
    {
        if (entry.factory == nullptr)
        {
            DEBUG_WARN("Null factory for window: " << entry.windowName);
            continue;
        }

        windows.push_back(entry.factory());
    }

    return windows;
}