#pragma once

#include "../../EnginePluginAPI.h"
#include "../Inspector/IInspectorPreviewRenderer.h"
#include "PreviewRenderTarget.h"

#include <memory>
#include <string>
#include <unordered_map>

class Material;
class Texture;
class MeshModel;

class ENGINE_API MaterialSpherePreviewRenderer : public IInspectorPreviewRenderer
{
public:
    MaterialSpherePreviewRenderer();
    ~MaterialSpherePreviewRenderer() override;

    void SetMaterial(
        Material* material,
        GLuint shaderProgram
    );

    GLuint RenderPreview(
        EditorContext& context,
        int width,
        int height
    ) override;

private:
    Material* material = nullptr;
    GLuint shaderProgram = 0;

    PreviewRenderTarget renderTarget;

    std::unique_ptr<MeshModel> sphereMesh;

    std::unordered_map<std::string, std::unique_ptr<Texture>> loadedTextures;

    void EnsureSphereMesh();
    void DestroySphereMesh();

    void SetPreviewUniforms(int width, int height);
    void BindMaterialTextures();

    Texture* GetOrLoadTexture(const std::string& texturePath);
};