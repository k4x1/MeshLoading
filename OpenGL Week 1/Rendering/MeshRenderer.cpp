#include "MeshRenderer.h"

#include "ShaderLoader.h"
#include "MaterialManager.h"
#include "../ObjectSystem/ComponentFactory.h"
#include "../Editor/InspectorSlotRegistry.h"
#include "../ObjectSystem/GameObject.h"
#include "../Utils/Debug.h"
#include "Camera.h"

#include <gtc/type_ptr.hpp>
#include <imgui.h>
#include <filesystem>


namespace fs = std::filesystem;

MeshRenderer::MeshRenderer(
    const glm::vec3& pos,
    const glm::vec3& rot,
    const glm::vec3& scl,
    const std::string& modelPath
)
    : modelFilePath(modelPath)
{
    Reload();
}
void MeshRenderer::Update(float dt)
{
    CheckFileUpdates();

    if (owner != nullptr && mesh != nullptr)
    {
        mesh->m_modelMatrix = owner->GetWorldMatrix();
    }
}
void MeshRenderer::OnInspectorGUI()
{
    char buffer[256];

    strncpy_s(buffer, modelFilePath.c_str(), sizeof(buffer));

    if (ImGui::InputText("Model (.obj)", buffer, sizeof(buffer)))
    {
        modelFilePath = buffer;
        Reload();
    }

    strncpy_s(buffer, materialFilePath.c_str(), sizeof(buffer));

    if (ImGui::InputText("Material", buffer, sizeof(buffer)))
    {
        materialFilePath = buffer;
        Reload();
    }
}
void MeshRenderer::Render(Camera* cam)
{
    if (mesh == nullptr || shaderProgram == 0 || cam == nullptr)
    {
        return;
    }

    glUseProgram(shaderProgram);

    GLint vpLocation = glGetUniformLocation(shaderProgram, "VPMatrix");

    if (vpLocation >= 0)
    {
        glm::mat4 vpMatrix = cam->GetProjectionMatrix() * cam->GetViewMatrix();
        glUniformMatrix4fv(vpLocation, 1, GL_FALSE, glm::value_ptr(vpMatrix));
    }

    GLint modelLocation = glGetUniformLocation(shaderProgram, "ModelMat");

    if (modelLocation >= 0 && owner != nullptr)
    {
        glm::mat4 modelMatrix = owner->GetWorldMatrix();
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    if (material != nullptr)
    {
        material->ApplyPropertiesToShader(shaderProgram);
        BindMaterialTextures();
    }

    glBindVertexArray(mesh->GetVAO());
    glDrawArrays(mesh->GetDrawType(), 0, mesh->GetDrawCount());
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
void MeshRenderer::Reload()
{
    if (shaderProgram != 0)
    {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    if (modelFilePath.empty() == false && fs::exists(modelFilePath))
    {
        mesh = std::make_unique<MeshModel>(
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(1),
            modelFilePath
        );

        modelTime = fs::last_write_time(modelFilePath);
    }
    else
    {
        DEBUG_WARN("Invalid model path: " << modelFilePath);
    }

    ReloadMaterial();

    if (mesh != nullptr && shaderProgram != 0)
    {
        mesh->SetShader(shaderProgram);
    }
}
void MeshRenderer::ReloadMaterial()
{
    material = nullptr;
    loadedTextures.clear();

    if (materialFilePath.empty())
    {
        DEBUG_WARN("No material assigned.");
        return;
    }

    if (fs::exists(materialFilePath) == false)
    {
        DEBUG_ERR("Material file does not exist: " << materialFilePath);
        return;
    }

    material = MaterialManager::GetMaterial(materialFilePath);

    if (material == nullptr)
    {
        return;
    }

    materialTime = fs::last_write_time(materialFilePath);

    shaderProgram = ShaderLoader::CreateProgram(
        material->vertShaderPath.c_str(),
        material->fragShaderPath.c_str()
    );

    if (shaderProgram == 0)
    {
        DEBUG_ERR("Failed to create shader from material: " << materialFilePath);
        return;
    }

    material->SyncPropertiesWithShader(shaderProgram);

    if (fs::exists(material->vertShaderPath))
    {
        vertShaderTime = fs::last_write_time(material->vertShaderPath);
    }

    if (fs::exists(material->fragShaderPath))
    {
        fragShaderTime = fs::last_write_time(material->fragShaderPath);
    }
}
void MeshRenderer::CheckFileUpdates()
{
    try
    {
        if (modelFilePath.empty() == false &&
            fs::exists(modelFilePath) &&
            fs::last_write_time(modelFilePath) != modelTime)
        {
            Reload();
            return;
        }

        if (materialFilePath.empty() == false &&
            fs::exists(materialFilePath) &&
            fs::last_write_time(materialFilePath) != materialTime)
        {
            MaterialManager::ReloadMaterial(materialFilePath);
            Reload();
            return;
        }

        if (material != nullptr)
        {
            if (fs::exists(material->vertShaderPath) &&
                fs::last_write_time(material->vertShaderPath) != vertShaderTime)
            {
                Reload();
                return;
            }

            if (fs::exists(material->fragShaderPath) &&
                fs::last_write_time(material->fragShaderPath) != fragShaderTime)
            {
                Reload();
                return;
            }
        }
    }
    catch (const fs::filesystem_error&)
    {
    }
}
Texture* MeshRenderer::GetOrLoadTexture(const std::string& texturePath)
{
    std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator iterator =
        loadedTextures.find(texturePath);

    if (iterator != loadedTextures.end())
    {
        return iterator->second.get();
    }

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    texture->InitTexture(texturePath.c_str());

    Texture* texturePointer = texture.get();
    loadedTextures[texturePath] = std::move(texture);

    return texturePointer;
}

void MeshRenderer::BindMaterialTextures()
{
    if (material == nullptr)
    {
        return;
    }

    int textureSlot = 0;

    for (const std::pair<const std::string, MaterialPropertyValue>& propertyPair : material->properties)
    {
        const std::string& propertyName = propertyPair.first;
        const MaterialPropertyValue& value = propertyPair.second;

        if (value.type != ShaderPropertyType::Texture2D)
        {
            continue;
        }

        std::string texturePath = value.texturePath;

        if (texturePath.empty())
        {
            texturePath = "Resources/Textures/blankTex.png";
        }

        Texture* texture = GetOrLoadTexture(texturePath);

        if (texture == nullptr)
        {
            continue;
        }

        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, texture->GetId());

        GLint location = glGetUniformLocation(shaderProgram, propertyName.c_str());

        if (location >= 0)
        {
            glUniform1i(location, textureSlot);
        }

        textureSlot++;
    }
}
REGISTER_SERIALIZABLE_COMPONENT(MeshRenderer);

static bool meshInspectorSlots = []
{
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Model",
        [](MeshRenderer* meshRenderer)
        {
            return meshRenderer->modelFilePath;
        },
        [](MeshRenderer* meshRenderer, Asset const& asset)
        {
            meshRenderer->modelFilePath = asset.path;
            meshRenderer->Reload();
        },
        { AssetType::Model }
    );

    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Material",
        [](MeshRenderer* meshRenderer)
        {
            return meshRenderer->materialFilePath;
        },
        [](MeshRenderer* meshRenderer, Asset const& asset)
        {
            meshRenderer->materialFilePath = asset.path;
            meshRenderer->Reload();
        },
        { AssetType::Material }
    );

    return true;
}();