#include "MeshRenderer.h"
#include "GameObject.h"  
#include "Camera.h"
#include <glew.h>     
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "IInspectable.h"
MeshRenderer::MeshRenderer(const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const std::string& modelFilePath)
    : MeshModel(position, rotation, scale, modelFilePath)
{
}

void MeshRenderer::Update(float deltaTime)
{
    MeshModel::Update(deltaTime);
}

void MeshRenderer::Render(Camera* cam)
{
    glm::mat4 worldMatrix = owner ? owner->GetWorldMatrix() : CalculateModelMatrix();

    GLuint shader = GetShader();
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));

    MeshModel::Render(shader);
}
void MeshRenderer::OnInspectorGUI() {
    ImGui::DragFloat("Movement Speed", &a, 0.1f);
    ImGui::DragFloat("Mouse Sensitivity", &b, 0.01f);
}
