// MeshRenderer.cpp
#include "MeshRenderer.h"
#include "ShaderLoader.h"
#include "ComponentFactory.h"
#include "InspectorSlotRegistry.h"
#include <tiny_obj_loader.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <filesystem>
#include <iostream>
#include "GameObject.h"
#include "Camera.h"

namespace fs = std::filesystem;

//— public API —//

MeshRenderer::MeshRenderer(const glm::vec3& pos,
    const glm::vec3& rot,
    const glm::vec3& scl,
    const std::string& modelPath)
    : modelFilePath(modelPath),
    textureFilePath("Resources/Textures/blankTex.png"),
    vertShaderPath("Resources/Shaders/Texture.vert"),
    fragShaderPath("Resources/Shaders/Texture.frag")
{
    Reload();
    if (fs::exists(modelFilePath))   _modelTime = fs::last_write_time(modelFilePath);
    if (fs::exists(textureFilePath)) _texTime = fs::last_write_time(textureFilePath);
    if (fs::exists(vertShaderPath))  _vertTime = fs::last_write_time(vertShaderPath);
    if (fs::exists(fragShaderPath))  _fragTime = fs::last_write_time(fragShaderPath);
}

void MeshRenderer::Reload() {
    if (!fs::exists(modelFilePath)) {
        std::cerr << "[MeshRenderer] invalid model path: " << modelFilePath << "\n";
        return;
    }

    if (shaderProgram) glDeleteProgram(shaderProgram);

    mesh = std::make_unique<MeshModel>(glm::vec3(0), glm::vec3(0), glm::vec3(1), modelFilePath);
    texture.InitTexture(textureFilePath.c_str());
    textureID = texture.GetId();
    mesh->SetTexture(textureID);

    shaderProgram = ShaderLoader::CreateProgram(vertShaderPath.c_str(), fragShaderPath.c_str());
    mesh->SetShader(shaderProgram);
}

void MeshRenderer::_checkFileUpdates() {
    try {
        if (fs::last_write_time(modelFilePath) != _modelTime) {
            _modelTime = fs::last_write_time(modelFilePath);
            Reload();
        }
        if (fs::last_write_time(textureFilePath) != _texTime) {
            _texTime = fs::last_write_time(textureFilePath);
            Reload();
        }
        if (fs::last_write_time(vertShaderPath) != _vertTime ||
            fs::last_write_time(fragShaderPath) != _fragTime)
        {
            _vertTime = fs::last_write_time(vertShaderPath);
            _fragTime = fs::last_write_time(fragShaderPath);
            Reload();
        }
    }
    catch (fs::filesystem_error&) { /* ignore */ }
}

void MeshRenderer::Update(float dt) {
    _checkFileUpdates();
    if (owner && mesh)
        mesh->m_modelMatrix = owner->GetWorldMatrix();
}

void MeshRenderer::Render(Camera* cam) {
    if (!mesh || !shaderProgram) return;

    glUseProgram(shaderProgram);

    // VP
    if (GLint loc = glGetUniformLocation(shaderProgram, "VPMatrix"); loc >= 0) {
        glm::mat4 VP = cam->GetProjectionMatrix() * cam->GetViewMatrix();
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(VP));
    }

    // Model
    if (GLint loc = glGetUniformLocation(shaderProgram, "ModelMat"); loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mesh->m_modelMatrix));
    }

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (GLint loc = glGetUniformLocation(shaderProgram, "Texture0"); loc >= 0) {
        glUniform1i(loc, 0);
    }

    glBindVertexArray(mesh->GetVAO());
    glDrawArrays(mesh->GetDrawType(), 0, mesh->GetDrawCount());
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void MeshRenderer::OnInspectorGUI() {
    char buf[256];

    // Model path
    strncpy_s(buf, modelFilePath.c_str(), sizeof(buf));
    if (ImGui::InputText("Model (.obj)", buf, sizeof(buf))) {
        modelFilePath = buf; Reload(); _modelTime = fs::last_write_time(modelFilePath);
    }

    // Texture path
    strncpy_s(buf, textureFilePath.c_str(), sizeof(buf));
    if (ImGui::InputText("Texture file", buf, sizeof(buf))) {
        textureFilePath = buf; Reload(); _texTime = fs::last_write_time(textureFilePath);
    }

    // Vertex shader
    strncpy_s(buf, vertShaderPath.c_str(), sizeof(buf));
    if (ImGui::InputText("Vertex Shader", buf, sizeof(buf))) {
        vertShaderPath = buf; Reload(); _vertTime = fs::last_write_time(vertShaderPath);
    }

    // Fragment shader
    strncpy_s(buf, fragShaderPath.c_str(), sizeof(buf));
    if (ImGui::InputText("Fragment Shader", buf, sizeof(buf))) {
        fragShaderPath = buf; Reload(); _fragTime = fs::last_write_time(fragShaderPath);
    }
}
REGISTER_SERIALIZABLE_COMPONENT(MeshRenderer);

static bool _meshInspectorSlots = [] {
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Model",
        [](MeshRenderer* mr) { return mr->modelFilePath; },
        [](MeshRenderer* mr, Asset const& a) {
            mr->modelFilePath = a.path; mr->Reload();
        },
        { AssetType::Model }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Texture",
        [](MeshRenderer* mr) { return mr->textureFilePath; },
        [](MeshRenderer* mr, Asset const& a) {
            mr->textureFilePath = a.path; mr->Reload();
        },
        { AssetType::Texture }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Vertex Shader",
        [](MeshRenderer* mr) { return mr->vertShaderPath; },
        [](MeshRenderer* mr, Asset const& a) {
            mr->vertShaderPath = a.path; mr->Reload();
        },
        { AssetType::Script }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Fragment Shader",
        [](MeshRenderer* mr) { return mr->fragShaderPath; },
        [](MeshRenderer* mr, Asset const& a) {
            mr->fragShaderPath = a.path; mr->Reload();
        },
        { AssetType::Script }
    );
    return true;
    }();