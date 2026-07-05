#pragma once

#include "../EnginePluginAPI.h"
#include "../Editor/Inspector/IAssetInspector.h"
#include "../Editor/Preview/MaterialSpherePreviewRenderer.h"
#include <glew.h>

#include <string>

class Material;

class ENGINE_API MaterialAssetInspector : public IAssetInspector
{
public:

    bool CanInspect(const Asset& asset) const override;
    void Draw(EditorContext& context, const Asset& asset) override;

private:
    GLuint materialInspectorShaderProgram = 0;

    std::string cachedMaterialPath = "";
    std::string cachedVertShaderPath = "";
    std::string cachedFragShaderPath = "";
    
    MaterialSpherePreviewRenderer materialPreviewRenderer;
    
    GLuint GetShaderProgram(Material* material);
    void ClearShaderProgram();

    void DrawHeader(Material* material);
    void DrawSaveControls(Material* material);
};