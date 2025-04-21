#include "MeshRenderer.h"
#include "ShaderLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>
#include "UIHelpers.h"
#include "ComponentFactory.h"
#include "Debug.h"      
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
    _modelTime = fs::last_write_time(modelFilePath);
    _texTime = fs::last_write_time(textureFilePath);
    _vertTime = fs::last_write_time(vertShaderPath);
    _fragTime = fs::last_write_time(fragShaderPath);
}

void MeshRenderer::Reload() {
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
    if (owner)
        mesh->m_modelMatrix = owner->GetWorldMatrix();
}

void MeshRenderer::Render(Camera* cam) {
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
    ComponentFactory::Instance().Register(
        "MeshRenderer",

        // creator: read either the new keys or fall back to the old ones
        [](const nlohmann::json& js, GameObject* owner)->Component* {
            std::string modelPath = js.value("modelFilePath",
                js.value("model", ""));
            std::string texPath = js.value("textureFilePath",
                js.value("texture", "Resources/Textures/blankTex.png"));
            std::string vertPath = js.value("vertShaderPath",
                js.value("vertShader", "Resources/Shaders/Texture.vert"));
            std::string fragPath = js.value("fragShaderPath",
                js.value("fragShader", "Resources/Shaders/Texture.frag"));

            auto* mr = owner->AddComponent<MeshRenderer>(
                glm::vec3(0), glm::vec3(0), glm::vec3(1),
                modelPath
            );
            mr->textureFilePath = texPath;
            mr->vertShaderPath = vertPath;
            mr->fragShaderPath = fragPath;
            mr->Reload();
            return mr;
        },

        // serializer: only write the new, unambiguous keys
        [](Component* comp)->nlohmann::json {
            if (auto* mr = dynamic_cast<MeshRenderer*>(comp)) {
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
    return true;
    }();
