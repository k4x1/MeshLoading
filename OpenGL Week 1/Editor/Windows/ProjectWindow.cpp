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
    
    DrawFolder(fs::path("Assets"));

    if (shouldOpenCreatePopup)
    {
        ImGui::OpenPopup("Create Project Asset");
        shouldOpenCreatePopup = false;
    }

    
    DrawCreatePopup();

    ImGui::End();
}

void ProjectWindow::DrawFolder(const fs::path& directoryPath)
{
    for (auto& entry : fs::directory_iterator(directoryPath)) {
                const auto name = entry.path().filename().string();

                if (entry.is_directory()) {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                    ImGui::PushID(entry.path().string().c_str());
                    bool open = ImGui::TreeNodeEx(name.c_str(), flags, "%s/", name.c_str());

                    if (ImGui::BeginPopupContextItem()) {
                        
                        DrawCreateMenu(entry.path());
                        ImGui::Separator();
                        if (ImGui::MenuItem("Open in Explorer"))
                        {
                            OpenInExplorer(entry.path());
                        }

                        ImGui::Separator();
                        if (ImGui::MenuItem("Rename Folder")) {
                            // TODO: implement rename-folder logic
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
                        DrawFolder(entry.path());
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
                    ImGui::Selectable(name.c_str(), false);

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

                      
                        ImGui::EndPopup();
                        
                        ImGui::Separator();
                        
                        DrawCreateMenu(entry.path());
                        
                        ImGui::Separator();
                        
                        if (ImGui::MenuItem("Rename File")) {
                            // TODO: implement rename-file logic
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
                if (createPopupMode == CreatePopupMode::Folder)
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

            if (createPopupMode == CreatePopupMode::Folder)
            {
                AssetManager::CreateFolder(createDirectory, finalName);
            }
            else if (createPopupMode == CreatePopupMode::Asset)
            {
                AssetManager::CreateAsset(createAssetType, createDirectory, finalName);
            }

            createPopupMode = CreatePopupMode::None;
            createName[0] = '\0';

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(80, 0)))
        {
            createPopupMode = CreatePopupMode::None;
            createName[0] = '\0';

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ProjectWindow::OpenCreateFolderPopup(const fs::path& directoryPath)
{
    createPopupMode = CreatePopupMode::Folder;
    createDirectory = directoryPath;
    createName[0] = '\0';
    shouldOpenCreatePopup = true;
    
    ImGui::OpenPopup("Create Project Asset");
}

void ProjectWindow::OpenCreateAssetPopup(const fs::path& directoryPath, AssetType assetType)
{
    createPopupMode = CreatePopupMode::Asset;
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
