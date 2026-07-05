#pragma once

#include "MeshModel.h"
#include "../ObjectSystem/Component.h"
#include "Material.h"
#include "Texture.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include <glm.hpp>

#include "Material.h"

namespace fs = std::filesystem;

class ENGINE_API MeshRenderer : public ISerializableComponent
{
public:
    MeshRenderer(
        const glm::vec3& pos = glm::vec3(0),
        const glm::vec3& rot = glm::vec3(0),
        const glm::vec3& scl = glm::vec3(1),
        const std::string& modelPath = ""
    );

    void Update(float dt) override;
    void Render(Camera* cam) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override
    {
        return {
                { "modelFilePath", modelFilePath },
                { "materialFilePath", materialFilePath },
        };
    }

    void Deserialize(const nlohmann::json& json) override
    {
        modelFilePath = json.value("modelFilePath", modelFilePath);
        materialFilePath = json.value("materialFilePath", materialFilePath);

        Reload();
    }

    std::string modelFilePath;
    std::string materialFilePath = "Assets/Materials/Default.mat";

    void Reload();

private:
    std::unique_ptr<MeshModel> mesh;

    Material* material = nullptr;

    GLuint shaderProgram = 0;

    std::unordered_map<std::string, std::unique_ptr<Texture>> loadedTextures;

    fs::file_time_type modelTime;
    fs::file_time_type materialTime;
    fs::file_time_type vertShaderTime;
    fs::file_time_type fragShaderTime;

    void CheckFileUpdates();

    void ReloadMaterial();
    Texture* GetOrLoadTexture(const std::string& texturePath);
    void BindMaterialTextures();
};