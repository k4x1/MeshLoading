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
    const std::string& modelFilePath)
    : MeshModel(position, rotation, scale, modelFilePath)
{}

void MeshRenderer::Update(float deltaTime) {
    MeshModel::Update(deltaTime);
}

void MeshRenderer::Render(Camera* cam) {
    // world & VP matrices
    glm::mat4 world = owner ? owner->GetWorldMatrix() : CalculateModelMatrix();
    glm::mat4 VP = cam->GetProjectionMatrix() * cam->GetViewMatrix();

    GLuint shader = GetShader();
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "VPMatrix"),
        1, GL_FALSE, glm::value_ptr(VP));
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"),
        1, GL_FALSE, glm::value_ptr(world));

    BindTexture();
    MeshModel::Render(shader);
}

void MeshRenderer::OnInspectorGUI() {
    ImGui::DragFloat("Movement Speed", &a, 0.1f);
    ImGui::DragFloat("Mouse Sensitivity", &b, 0.01f);
}

// ——— ComponentFactory registration ———
static bool meshReg = []() {
    ComponentFactory::Instance().Register("MeshRenderer",
        [&](const nlohmann::json& js, GameObject* owner)->Component* {
            // parse JSON
            glm::vec3 pos{/*…*/ }, rot{/*…*/ }, scl{/*…*/ };
            std::string model = js.value("model", "");
            std::string tex = js.value("texture", "");

            // add component
            auto* mr = owner->AddComponent<MeshRenderer>(pos, rot, scl, model);

            // load texture
            Texture tmp;
            tmp.InitTexture(tex.c_str());
            mr->SetTexture(tmp.GetId());

            // compile & assign shader
            GLuint programTexture = ShaderLoader::CreateProgram(
                "Resources/Shaders/Texture.vert",
                "Resources/Shaders/Texture.frag"
            );
            mr->SetShader(programTexture);

            return mr;
        }
    );
    return true;
    }();
