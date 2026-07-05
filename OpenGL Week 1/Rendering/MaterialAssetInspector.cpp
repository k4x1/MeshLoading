#include "MaterialAssetInspector.h"

#include "../Editor/Inspector/AssetInspectorRegistry.h"
#include "../Editor/Inspector/InspectorPropertyList.h"
#include "../Editor/Inspector/InspectorPropertyDrawer.h"

#include "Material.h"
#include "MaterialManager.h"

#include "ShaderLoader.h"

#include "../Utils/Debug.h"
#include <imgui.h>

#include "Editor/Inspector/InspectorPreview.h"

void MaterialAssetInspector::Draw(EditorContext& context, const Asset& asset)
{
    Material* material = MaterialManager::GetMaterial(asset.path);

    if (material == nullptr)
    {
        ImGui::TextUnformatted("Failed to load material.");
        ImGui::TextWrapped("%s", asset.path.c_str());
        return;
    }

    GLuint shaderProgram = GetShaderProgram(material);

    DrawHeader(material);
    
    std::string previousVertShaderPath = material->vertShaderPath;
    std::string previousFragShaderPath = material->fragShaderPath;

    InspectorPropertyList propertyList;
    material->BuildInspectorProperties(propertyList, shaderProgram);

    InspectorPropertyDrawer::DrawProperties(propertyList);

    if (previousVertShaderPath != material->vertShaderPath ||
        previousFragShaderPath != material->fragShaderPath)
    {
        ClearShaderProgram();
    }

    ImGui::Separator();

    if (shaderProgram == 0)
    {
        ImGui::TextWrapped("Shader failed to compile or could not be loaded.");
    }
    ImGui::Separator();
    DrawSaveControls(material);
    ImGui::Separator();
    materialPreviewRenderer.SetMaterial(
          material,
          shaderProgram
      );
    
    InspectorPreview::DrawPreview(
        "Preview",
        materialPreviewRenderer,
        context
    );
}
void MaterialAssetInspector::DrawHeader(Material* material)
{
    ImGui::TextUnformatted("Material");
    ImGui::Separator();

    if (material == nullptr)
    {
        return;
    }

    ImGui::TextWrapped("%s", material->path.c_str());

    ImGui::Separator();
}
void MaterialAssetInspector::DrawSaveControls(Material* material)
{
    if (material == nullptr)
    {
        return;
    }

    if (material->isDirty)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.8f, 0.1f, 1.0f),
            "Unsaved changes"
        );
    }

    if (ImGui::Button("Save Material"))
    {
        if (material->SaveToFile(material->path))
        {
            material->isDirty = false;
            DEBUG_LOG("Saved material: " << material->path);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reload"))
    {
        MaterialManager::ReloadMaterial(material->path);
        ClearShaderProgram();

        DEBUG_LOG("Reloaded material: " << material->path);
    }
}

GLuint MaterialAssetInspector::GetShaderProgram(Material* material)
{
    if (material == nullptr)
    {
        return 0;
    }

    bool shouldRebuild = false;

    if (materialInspectorShaderProgram == 0)
    {
        shouldRebuild = true;
    }

    if (cachedMaterialPath != material->path)
    {
        shouldRebuild = true;
    }

    if (cachedVertShaderPath != material->vertShaderPath)
    {
        shouldRebuild = true;
    }

    if (cachedFragShaderPath != material->fragShaderPath)
    {
        shouldRebuild = true;
    }

    if (shouldRebuild == false)
    {
        return materialInspectorShaderProgram;
    }

    ClearShaderProgram();

    materialInspectorShaderProgram = ShaderLoader::CreateProgram(
        material->vertShaderPath.c_str(),
        material->fragShaderPath.c_str()
    );

    if (materialInspectorShaderProgram == 0)
    {
        DEBUG_ERR("Failed to compile material inspector shader: " << material->path);
        return 0;
    }

    cachedMaterialPath = material->path;
    cachedVertShaderPath = material->vertShaderPath;
    cachedFragShaderPath = material->fragShaderPath;

    material->SyncPropertiesWithShader(materialInspectorShaderProgram);

    return materialInspectorShaderProgram;
}
void MaterialAssetInspector::ClearShaderProgram()
{
    if (materialInspectorShaderProgram != 0)
    {
        glDeleteProgram(materialInspectorShaderProgram);
        materialInspectorShaderProgram = 0;
    }

    cachedMaterialPath.clear();
    cachedVertShaderPath.clear();
    cachedFragShaderPath.clear();
}
bool MaterialAssetInspector::CanInspect(const Asset& asset) const
{
    //rework to use extension paths
    return asset.type == AssetType::Material;
}
REGISTER_ASSET_INSPECTOR(MaterialAssetInspector);