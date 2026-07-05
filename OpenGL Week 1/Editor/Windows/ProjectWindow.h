#pragma once

#include "../../EnginePluginAPI.h"
#include "../../ObjectSystem/AssetManager.h"
#include "../IEditorWindow.h"

#include <filesystem>


namespace fs = std::filesystem;

class ENGINE_API ProjectWindow : public IEditorWindow
{
public:
    void Draw(EditorContext& context) override;
    const char* GetWindowName() const override;
private:
    enum class PopupMode
    {
        None,
        Folder,
        Asset
    };

    PopupMode createPopupMode = PopupMode::None;
    AssetType createAssetType = AssetType::Script;
    fs::path createDirectory = "Assets";
    char createName[128] = "";
    bool shouldOpenCreatePopup = false;
    
    PopupMode renamePopupMode = PopupMode::None;
    fs::path renamePath;
    char renameName[128] = "";
    bool shouldOpenRenamePopup = false;

    
    void DrawFolder(EditorContext& context, const fs::path& directoryPath);
    void OpenInExplorer(const fs::path& path);


    void DrawRenamePopup();
    void OpenRenamePopup(const fs::path& path);
    void DrawCreateMenu(const fs::path& directoryPath);
    void DrawCreatePopup();

    void OpenCreateFolderPopup(const fs::path& directoryPath);
    void OpenCreateAssetPopup(const fs::path& directoryPath, AssetType assetType);
};