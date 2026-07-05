#include "ProjectWindow.h"
#include "imgui.h"
#include "../../ObjectSystem/GameObject.h"
#include "../../Utils/Debug.h"
#include "../../ObjectSystem/PrefabSystem.h"
#include "Editor/EditorWindowRegistry.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <shellapi.h>
#endif

REGISTER_EDITOR_WINDOW(ProjectWindow);


void ProjectWindow::Draw(EditorContext& context) {
    ImGui::Begin("Project");

    if (ImGui::BeginPopupContextWindow(
    "ProjectWindowContextMenu",
    ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        DrawCreateMenu("Assets");
        ImGui::Separator();
        if (ImGui::MenuItem("Open in Explorer"))
        {
            OpenInExplorer("Assets");
        }
        ImGui::EndPopup();
    }
    

    ImGui::TextUnformatted("Drop a GameObject here to save a Prefab");
    if (ImGui::BeginDragDropTarget()) {
        if (auto pl = ImGui::AcceptDragDropPayload("DND_GAMEOBJECT")) {
            GameObject* go = *(GameObject**)pl->Data;
            
            std::string safe = go->name;
            
            for (char& c : safe) if (c == ' ') c = '_';
            
            std::string path = "Assets/Prefabs/" + safe + ".prefab";
            
            DEBUG_LOG("Saving Prefab: " << path << "\n");
            
            if (PrefabSystem::SavePrefab(go, path))
                AssetManager::LoadAssets("Assets");
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Separator();
    
    DrawFolder(context, fs::path("Assets"));

    if (shouldOpenCreatePopup)
    {
        ImGui::OpenPopup("Create Project Asset");
        shouldOpenCreatePopup = false;
    }

    DrawCreatePopup();
    
    if (shouldOpenRenamePopup)
    {
        ImGui::OpenPopup("Rename Project Item");
        shouldOpenRenamePopup = false;
    }

    DrawRenamePopup();

    

    ImGui::End();
}

void ProjectWindow::DrawFolder(EditorContext& context, const fs::path& directoryPath)
{
    for (auto& entry : fs::directory_iterator(directoryPath)) {
                const auto name = entry.path().filename().string();

                if (entry.is_directory()) {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                    ImGui::PushID(entry.path().string().c_str());
                    bool open = ImGui::TreeNodeEx(name.c_str(), flags, "%s/", name.c_str());

                    if (ImGui::BeginPopupContextItem()) {
                        
                        if (ImGui::MenuItem("Open in Explorer"))
                        {
                            OpenInExplorer(entry.path());
                        }

                        ImGui::Separator();
                        if (ImGui::MenuItem("Rename Folder")) {
                                OpenRenamePopup(entry.path());
                        }
                        if (ImGui::MenuItem("Delete Folder")) {
                            std::error_code ec;
                            fs::remove_all(entry.path(), ec);
                            if (ec)
                                DEBUG_ERR(" Failed to delete folder: "
                                << entry.path().string() << "\n");
                            AssetManager::LoadAssets("Assets");
                        }
                        ImGui::EndPopup();
                    }

                    if (open) {
                        DrawFolder(context, entry.path());
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                else {
                    ImGui::PushID(entry.path().string().c_str());
                    if (AssetManager::IsHiddenExtension(entry.path()))
                    {
                        ImGui::PopID();
                        continue;
                    }
                    
                    bool isSelected = false;

                    if (context.selectedAssetPath != nullptr)
                    {
                        isSelected = *context.selectedAssetPath == entry.path().string();
                    }
                    
                    if (ImGui::Selectable(name.c_str(), isSelected))
                    {
                        DEBUG_LOG("Selected Asset: " << entry.path().string() << "\n");
                        if (context.selectedAssetPath != nullptr)
                        {
                            *context.selectedAssetPath = entry.path().string();
                        }

                        if (context.selectedGameObject != nullptr)
                        {
                            *context.selectedGameObject = nullptr;
                        }
                    }

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                        const auto& assets = AssetManager::GetAssets();
                        for (int i = 0; i < (int)assets.size(); ++i) {
                            if (assets[i].path == entry.path().string()) {
                                ImGui::SetDragDropPayload("ASSET_PAYLOAD", &i, sizeof(i));
                                ImGui::Text("%s", name.c_str());
                                break;
                            }
                        }
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Open in Explorer"))
                        {
                            OpenInExplorer(entry.path());
                        }
                        
                        ImGui::Separator();
                        
                        if (ImGui::MenuItem("Rename File")) {
                            OpenRenamePopup(entry.path());
                        }
                        if (ImGui::MenuItem("Delete File")) {
                            std::error_code ec;
                            fs::remove(entry.path(), ec);
                            if (ec)
                                DEBUG_ERR("[UIHelpers] Failed to delete file: "
                                << entry.path().string() << "\n");
                            AssetManager::LoadAssets("Assets");
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
            }
}
void ProjectWindow::OpenRenamePopup(const fs::path& path)
{
    renamePath = path;

    if (fs::is_directory(path))
    {
        renamePopupMode = PopupMode::Folder;
    }
    else
    {
        renamePopupMode = PopupMode::Asset;
    }

    std::string currentName;

    if (renamePopupMode == PopupMode::Asset)
    {
        currentName = path.stem().string();
    }
    else
    {
        currentName = path.filename().string();
    }

    strncpy_s(renameName, currentName.c_str(), sizeof(renameName));
    renameName[sizeof(renameName) - 1] = '\0';

    shouldOpenRenamePopup = true;
}

void ProjectWindow::DrawRenamePopup()
{
    if (ImGui::BeginPopupModal("Rename Project Item", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Rename:");
        ImGui::SameLine();
        ImGui::TextUnformatted(renamePath.filename().string().c_str());

        ImGui::InputText("Name", renameName, sizeof(renameName));

        if (ImGui::Button("Rename", ImVec2(80, 0)))
        {
            std::string finalName = renameName;

            if (finalName.empty() == false)
            {
                fs::path targetPath;

                if (renamePopupMode == PopupMode::Asset)
                {
                    fs::path typedPath = finalName;

                    if (typedPath.extension().empty())
                    {
                        targetPath = renamePath.parent_path() /
                            (finalName + renamePath.extension().string());
                    }
                    else
                    {
                        targetPath = renamePath.parent_path() / typedPath.filename();
                    }
                }
                else
                {
                    targetPath = renamePath.parent_path() / finalName;
                }

                if (targetPath == renamePath)
                {
                    renamePopupMode = PopupMode::None;
                    renameName[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                else if (fs::exists(targetPath))
                {
                    DEBUG_WARN("Cannot rename. Target already exists: " << targetPath.string());
                }
                else
                {
                    std::error_code ec;
                    fs::rename(renamePath, targetPath, ec);

                    if (ec)
                    {
                        DEBUG_ERR("Failed to rename: " << renamePath.string()
                            << " to " << targetPath.string()
                            << " error: " << ec.message());
                    }
                    else
                    {
                        DEBUG_LOG("Renamed: " << renamePath.string()
                            << " to " << targetPath.string());

                        AssetManager::LoadAssets("Assets");

                        renamePopupMode = PopupMode::None;
                        renameName[0] = '\0';

                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(80, 0)))
        {
            renamePopupMode = PopupMode::None;
            renameName[0] = '\0';

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ProjectWindow::OpenInExplorer(const fs::path& path)
{
#ifdef _WIN32
    fs::path absolutePath = fs::absolute(path);

    if (fs::is_directory(absolutePath))
    {
        HINSTANCE result = ShellExecuteA(
            nullptr,
            "open",
            absolutePath.string().c_str(),
            nullptr,
            nullptr,
            SW_SHOWDEFAULT
        );

        if (reinterpret_cast<intptr_t>(result) <= 32)
        {
            DEBUG_ERR("Failed to open folder in Explorer: " << absolutePath.string());
        }

        return;
    }

    if (fs::is_regular_file(absolutePath))
    {
        std::string parameters = "/select,\"" + absolutePath.string() + "\"";

        HINSTANCE result = ShellExecuteA(
            nullptr,
            "open",
            "explorer.exe",
            parameters.c_str(),
            nullptr,
            SW_SHOWDEFAULT
        );

        if (reinterpret_cast<intptr_t>(result) <= 32)
        {
            DEBUG_ERR("Failed to reveal file in Explorer: " << absolutePath.string());
        }

        return;
    }

    DEBUG_WARN("Cannot open missing path in Explorer: " << absolutePath.string());
#else
    DEBUG_WARN("OpenInExplorer is only implemented for Windows.");
#endif
}

void ProjectWindow::DrawCreateMenu(const fs::path& directoryPath)
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Folder"))
        {
            OpenCreateFolderPopup(directoryPath);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Script"))
        {
            OpenCreateAssetPopup(directoryPath, AssetType::Script);
        }

        if (ImGui::MenuItem("Material"))
        {
            OpenCreateAssetPopup(directoryPath, AssetType::Material);
        }

        ImGui::EndMenu();
    }
}

void ProjectWindow::DrawCreatePopup()
{
    if (ImGui::BeginPopupModal("Create Project Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Create in:");
        ImGui::SameLine();
        ImGui::TextUnformatted(createDirectory.string().c_str());

        ImGui::InputText("Name", createName, sizeof(createName));

        if (ImGui::Button("Create", ImVec2(80, 0)))
        {
            std::string finalName = createName;

            if (finalName.empty())
            {
                if (createPopupMode == PopupMode::Folder)
                {
                    finalName = "NewFolder";
                }
                else if (createAssetType == AssetType::Script)
                {
                    finalName = "NewComponent";
                }
                else if (createAssetType == AssetType::Material)
                {
                    finalName = "NewMaterial";
                }
                else
                {
                    finalName = "NewAsset";
                }
            }

            if (createPopupMode == PopupMode::Folder)
            {
                AssetManager::CreateFolder(createDirectory, finalName);
            }
            else if (createPopupMode == PopupMode::Asset)
            {
                AssetManager::CreateAsset(createAssetType, createDirectory, finalName);
            }

            createPopupMode = PopupMode::None;
            createName[0] = '\0';

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(80, 0)))
        {
            createPopupMode = PopupMode::None;
            createName[0] = '\0';

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ProjectWindow::OpenCreateFolderPopup(const fs::path& directoryPath)
{
    createPopupMode = PopupMode::Folder;
    createDirectory = directoryPath;
    createName[0] = '\0';
    shouldOpenCreatePopup = true;
    
    ImGui::OpenPopup("Create Project Asset");
}

void ProjectWindow::OpenCreateAssetPopup(const fs::path& directoryPath, AssetType assetType)
{
    createPopupMode = PopupMode::Asset;
    createAssetType = assetType;
    createDirectory = directoryPath;
    createName[0] = '\0';
    shouldOpenCreatePopup = true;
    
    ImGui::OpenPopup("Create Project Asset");
}

const char* ProjectWindow::GetWindowName() const
{
    return "Project";
}
