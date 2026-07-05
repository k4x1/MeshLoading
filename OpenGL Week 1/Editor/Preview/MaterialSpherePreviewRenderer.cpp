#include "MaterialSpherePreviewRenderer.h"

#include "../../Rendering/Material.h"
#include "../../Rendering/MaterialPropertyValue.h"

#include "../../Rendering/MeshModel.h"
#include "../../Rendering/Texture.h"

#include "../../Utils/Debug.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <filesystem>

namespace fs = std::filesystem;

static const char* PreviewSpherePath = "Assets/Models/sphere.obj";

static void SetMat4IfExists(
    GLuint shaderProgram,
    const char* uniformName,
    const glm::mat4& value
)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);

    if (location >= 0)
    {
        glUniformMatrix4fv(
            location,
            1,
            GL_FALSE,
            glm::value_ptr(value)
        );
    }
}

static void SetVec3IfExists(
    GLuint shaderProgram,
    const char* uniformName,
    const glm::vec3& value
)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);

    if (location >= 0)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

static void SetFloatIfExists(
    GLuint shaderProgram,
    const char* uniformName,
    float value
)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);

    if (location >= 0)
    {
        glUniform1f(location, value);
    }
}

static void SetIntIfExists(
    GLuint shaderProgram,
    const char* uniformName,
    int value
)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);

    if (location >= 0)
    {
        glUniform1i(location, value);
    }
}

static void SetUIntIfExists(
    GLuint shaderProgram,
    const char* uniformName,
    unsigned int value
)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);

    if (location >= 0)
    {
        glUniform1ui(location, value);
    }
}
MaterialSpherePreviewRenderer::MaterialSpherePreviewRenderer() = default;


GLuint MaterialSpherePreviewRenderer::RenderPreview(
    EditorContext& context,
    int width,
    int height
)
{
    if (material == nullptr)
    {
        return 0;
    }

    if (shaderProgram == 0)
    {
        return 0;
    }

    if (width <= 0 || height <= 0)
    {
        return 0;
    }

    EnsureSphereMesh();

    if (sphereMesh == nullptr)
    {
        return 0;
    }

    renderTarget.EnsureSize(width, height);

    GLint previousProgram = 0;
    GLfloat previousClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);

    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, previousClearColor);

    renderTarget.Bind();

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.08f, 0.08f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    SetPreviewUniforms(width, height);

    material->ApplyPropertiesToShader(shaderProgram);
    BindMaterialTextures();

    glBindVertexArray(sphereMesh->GetVAO());
    glDrawArrays(
        sphereMesh->GetDrawType(),
        0,
        sphereMesh->GetDrawCount()
    );
    glBindVertexArray(0);

    glUseProgram(previousProgram);

    renderTarget.Unbind();

    glClearColor(
        previousClearColor[0],
        previousClearColor[1],
        previousClearColor[2],
        previousClearColor[3]
    );

    if (depthWasEnabled == GL_FALSE)
    {
        glDisable(GL_DEPTH_TEST);
    }

    return renderTarget.GetColorTexture();
}

void MaterialSpherePreviewRenderer::EnsureSphereMesh()
{
    if (sphereMesh != nullptr)
    {
        return;
    }

    if (fs::exists(PreviewSpherePath) == false)
    {
        DEBUG_ERR("Material preview sphere missing: " << PreviewSpherePath);
        return;
    }

    sphereMesh = std::make_unique<MeshModel>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(1.0f),
        PreviewSpherePath
    );
}

void MaterialSpherePreviewRenderer::SetPreviewUniforms(
    int width,
    int height
)
{
    if (shaderProgram == 0)
    {
        return;
    }

    float aspect = static_cast<float>(width) / static_cast<float>(height);

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(-15.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(35.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);

    glm::mat4 viewMatrix = glm::lookAt(
        cameraPosition,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(35.0f),
        aspect,
        0.1f,
        100.0f
    );

    glm::mat4 vpMatrix = projectionMatrix * viewMatrix;

    SetMat4IfExists(shaderProgram, "ModelMat", modelMatrix);
    SetMat4IfExists(shaderProgram, "VPMatrix", vpMatrix);
    SetVec3IfExists(shaderProgram, "CameraPos", cameraPosition);

    SetUIntIfExists(shaderProgram, "PointLightCount", 0);
    SetIntIfExists(shaderProgram, "PointLightCount", 0);

    SetVec3IfExists(
        shaderProgram,
        "DirLight.direction",
        glm::normalize(glm::vec3(-0.4f, -1.0f, -0.25f))
    );

    SetVec3IfExists(
        shaderProgram,
        "DirLight.color",
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    SetFloatIfExists(
        shaderProgram,
        "DirLight.specularStrength",
        0.5f
    );

    SetVec3IfExists(
        shaderProgram,
        "LightPos",
        glm::vec3(2.0f, 2.0f, 2.0f)
    );

    SetVec3IfExists(
        shaderProgram,
        "LightColor",
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
}

void MaterialSpherePreviewRenderer::BindMaterialTextures()
{
    if (material == nullptr)
    {
        return;
    }

    if (shaderProgram == 0)
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

        GLint location = glGetUniformLocation(
            shaderProgram,
            propertyName.c_str()
        );

        if (location >= 0)
        {
            glUniform1i(location, textureSlot);
        }

        textureSlot++;
    }
}
Texture* MaterialSpherePreviewRenderer::GetOrLoadTexture(
    const std::string& texturePath
)
{
    if (texturePath.empty())
    {
        return nullptr;
    }

    std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator iterator =
        loadedTextures.find(texturePath);

    if (iterator != loadedTextures.end())
    {
        return iterator->second.get();
    }

    if (fs::exists(texturePath) == false)
    {
        DEBUG_WARN("Preview texture path does not exist: " << texturePath);
        return nullptr;
    }

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    texture->InitTexture(texturePath.c_str());

    Texture* texturePointer = texture.get();
    loadedTextures[texturePath] = std::move(texture);

    return texturePointer;
}
void MaterialSpherePreviewRenderer::DestroySphereMesh()
{
    sphereMesh = nullptr;
}


void MaterialSpherePreviewRenderer::SetMaterial(
    Material* newMaterial,
    GLuint newShaderProgram
)
{
    material = newMaterial;
    shaderProgram = newShaderProgram;
}
MaterialSpherePreviewRenderer::~MaterialSpherePreviewRenderer()
{
    DestroySphereMesh();
}