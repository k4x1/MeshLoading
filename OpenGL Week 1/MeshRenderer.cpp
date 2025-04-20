#include "MeshRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include "ComponentFactory.h"
#include "ShaderLoader.h"

#include <glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>


MeshRenderer::MeshRenderer(const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const std::string& _modelFilePath)
    : MeshModel(position, rotation, scale, _modelFilePath),
    modelFilePath(_modelFilePath)   
{}

void MeshRenderer::Update(float deltaTime) {
    if (owner) {
        m_modelMatrix = owner->GetWorldMatrix();
    }
    else {
        m_modelMatrix = CalculateModelMatrix();
    }
    MeshModel::Update(deltaTime);
}

void MeshRenderer::Render(Camera* cam) {
    glm::mat4 VP = cam->GetProjectionMatrix() * cam->GetViewMatrix();

    GLuint shader = GetShader();
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "VPMatrix"),
        1, GL_FALSE, glm::value_ptr(VP));

    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"),
        1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    BindTexture();
    MeshModel::Render(shader);
}
void MeshRenderer::OnInspectorGUI() {
    ImGui::DragFloat("Movement Speed", &a, 0.1f);
    ImGui::DragFloat("Mouse Sensitivity", &b, 0.01f);
}
static bool meshReg = []() {
    ComponentFactory::Instance().Register(
        "MeshRenderer",

        // creator:
        [](const nlohmann::json& js, GameObject* owner)->Component* {
            glm::vec3 pos{/*…*/ }, rot{/*…*/ }, scl{/*…*/ };
            std::string model = js.value("model", "");
            std::string tex = js.value("texture", "");

            auto* mr = owner->AddComponent<MeshRenderer>(pos, rot, scl, model);
            mr->texturePath = tex;                     // ← store this too

            Texture tmp; tmp.InitTexture(tex.c_str());
            mr->SetTexture(tmp.GetId());

            GLuint prog = ShaderLoader::CreateProgram(
                "Resources/Shaders/Texture.vert",
                "Resources/Shaders/Texture.frag"
            );
            mr->SetShader(prog);
            return mr;
        },

        // serializer:
        [](Component* comp)->nlohmann::json {
            if (auto* mr = dynamic_cast<MeshRenderer*>(comp)) {
                return {
                    {"model",   mr->modelFilePath},
                    {"texture", mr->texturePath}
                };
            }
            return nullptr;
        }
    );
    return true;
    }();
