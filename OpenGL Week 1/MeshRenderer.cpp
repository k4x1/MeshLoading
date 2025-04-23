#include "MeshRenderer.h"
#include "ShaderLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>
#include "UIHelpers.h"
#include "ComponentFactory.h"
#include "Debug.h"      
#include "InspectorSlotRegistry.h"
MeshRenderer::MeshRenderer(const glm::vec3& pos,
    const glm::vec3& rot,
    const glm::vec3& scl,
    const std::string& modelPath)
    : modelFilePath(modelPath),
    textureFilePath("Resources/Textures/blankTex.png"),
    vertShaderPath("Resources/Shaders/Texture.vert"),
    fragShaderPath("Resources/Shaders/Texture.frag")
{
    Debug::Log("[MeshRenderer] Ctor: model=" + modelFilePath + " tex=" + textureFilePath);
    Reload();
    if (!modelFilePath.empty() && fs::exists(modelFilePath))
        _modelTime = fs::last_write_time(modelFilePath);
    if (!textureFilePath.empty() && fs::exists(textureFilePath))
        _texTime = fs::last_write_time(textureFilePath);
    if (!vertShaderPath.empty() && fs::exists(vertShaderPath))
        _vertTime = fs::last_write_time(vertShaderPath);
    if (!fragShaderPath.empty() && fs::exists(fragShaderPath))
        _fragTime = fs::last_write_time(fragShaderPath);
}

void MeshRenderer::Reload() {
    if (modelFilePath.empty() || !fs::exists(modelFilePath)) {
        Debug::LogWarning("[MeshRenderer] Reload skipped: invalid modelFilePath=\""
            + modelFilePath + '"');
        return;
    }
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
    }

    mesh = std::make_unique<MeshModel>(glm::vec3(0), glm::vec3(0), glm::vec3(1), modelFilePath);

    mesh->SetTexture(textureID);

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
    catch (fs::filesystem_error& e) {
        // ignore missing files / permissions errors
    }
}

void MeshRenderer::Update(float dt) {
    // 1) Watch disk for changes
    _checkFileUpdates();

    // 2) keep model matrix in sync
    if (owner && mesh)
        mesh->m_modelMatrix = owner->GetWorldMatrix();
}

void MeshRenderer::Render(Camera* cam) {
    if (!mesh || !shaderProgram)
        return;
    glUseProgram(shaderProgram);

    GLint locVP = glGetUniformLocation(shaderProgram, "VPMatrix");
    if (locVP >= 0) {
        glm::mat4 VP = cam->GetProjectionMatrix() * cam->GetViewMatrix();
        glUniformMatrix4fv(locVP, 1, GL_FALSE, glm::value_ptr(VP));
    }

    GLint locM = glGetUniformLocation(shaderProgram, "ModelMat");
    if (locM >= 0) {
        glUniformMatrix4fv(locM, 1, GL_FALSE, glm::value_ptr(mesh->m_modelMatrix));
    }

    // Texture binding
    GLint beforeBind = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &beforeBind);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint locTex = glGetUniformLocation(shaderProgram, "Texture0");
    if (locTex >= 0) {
        glUniform1i(locTex, 0);
    }

    GLint afterBind = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &afterBind);

    // Draw
    glBindVertexArray(mesh->GetVAO());
    glDrawArrays(mesh->GetDrawType(), 0, mesh->GetDrawCount());
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
void MeshRenderer::OnInspectorGUI() {
    // identical to before: edit paths & call Reload() immediately
    char buf[256];

    strncpy_s(buf, sizeof(buf), modelFilePath.c_str(), _TRUNCATE);
    if (ImGui::InputText("Model (.obj)", buf, sizeof(buf))) {
        modelFilePath = buf;
        Reload();
        _modelTime = fs::last_write_time(modelFilePath);
    }

    strncpy_s(buf, sizeof(buf), textureFilePath.c_str(), _TRUNCATE);
    if (ImGui::InputText("Texture file", buf, sizeof(buf))) {
        textureFilePath = buf;
        Reload();
        _texTime = fs::last_write_time(textureFilePath);
    }

    strncpy_s(buf, sizeof(buf), vertShaderPath.c_str(), _TRUNCATE);
    if (ImGui::InputText("Vertex Shader", buf, sizeof(buf))) {
        vertShaderPath = buf;
        Reload();
        _vertTime = fs::last_write_time(vertShaderPath);
    }

    strncpy_s(buf, sizeof(buf), fragShaderPath.c_str(), _TRUNCATE);
    if (ImGui::InputText("Fragment Shader", buf, sizeof(buf))) {
        fragShaderPath = buf;
        Reload();
        _fragTime = fs::last_write_time(fragShaderPath);
    }
}
static bool meshReg = []() {
    // 1) Tell the ComponentFactory about our type so
    //    "Add Component → MeshRenderer" will appear
    ComponentFactory::Instance().Register(
        "MeshRenderer",

        // how to create one from JSON when loading a scene
        [](const nlohmann::json& js, GameObject* owner)->Component* {
            std::string path = js.value("modelFilePath", "");
            auto* mr = owner->AddComponent<MeshRenderer>(
                glm::vec3(0), glm::vec3(0), glm::vec3(1),
                path
            );
            mr->textureFilePath = js.value("textureFilePath", "");
            mr->vertShaderPath = js.value("vertShaderPath", "");
            mr->fragShaderPath = js.value("fragShaderPath", "");
            mr->Reload();
            return mr;
        },

        // how to serialize it back to JSON when saving a scene
        [](Component* c)->nlohmann::json {
            if (auto* mr = dynamic_cast<MeshRenderer*>(c)) {
                return {
                    {"modelFilePath",   mr->modelFilePath},
                    {"textureFilePath", mr->textureFilePath},
                    {"vertShaderPath",  mr->vertShaderPath},
                    {"fragShaderPath",  mr->fragShaderPath}
                };
            }
            return nullptr;
        }
    );

    // 2) Hook up our Inspector‑slots so you can drag & drop assets
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Model",
        /* getter */   [](MeshRenderer* mr) { return mr->modelFilePath; },
        /* setter */   [](MeshRenderer* mr, const Asset& a) {
            mr->modelFilePath = a.path;
            mr->Reload();
        },
        /* accepts */{ AssetType::Model }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Texture",
        [](MeshRenderer* mr) { return mr->textureFilePath; },
        [](MeshRenderer* mr, const Asset& a) {
            mr->textureFilePath = a.path;
            mr->Reload();
        },
        { AssetType::Texture }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Vertex Shader",
        [](MeshRenderer* mr) { return mr->vertShaderPath; },
        [](MeshRenderer* mr, const Asset& a) {
            mr->vertShaderPath = a.path;
            mr->Reload();
        },
        { AssetType::Script }
    );
    InspectorSlotRegistry::RegisterSlot<MeshRenderer>(
        "Fragment Shader",
        [](MeshRenderer* mr) { return mr->fragShaderPath; },
        [](MeshRenderer* mr, const Asset& a) {
            mr->fragShaderPath = a.path;
            mr->Reload();
        },
        { AssetType::Script }
    );

    return true;
    }();