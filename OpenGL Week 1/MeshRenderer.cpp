// MeshRenderer.cpp
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include <glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "IInspectable.h"

MeshRenderer::MeshRenderer(const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const std::string& modelFilePath)
    : MeshModel(position, rotation, scale, modelFilePath)
{
}

void MeshRenderer::Update(float deltaTime) {
    // If needed, update any animation or model-specific logic.
    MeshModel::Update(deltaTime);
}

void MeshRenderer::Render(Camera* cam) {
    // Get the world transform from the game object's owner.
    glm::mat4 worldMatrix = owner ? owner->GetWorldMatrix() : CalculateModelMatrix();

    // Get the view and projection matrices from the camera.
    glm::mat4 view = cam->GetViewMatrix();
    glm::mat4 projection = cam->GetProjectionMatrix();
    glm::mat4 VP = projection * view;

    // Bind the shader program assigned to this component.
    GLuint shader = GetShader();
    glUseProgram(shader);

    // Set the combined view-projection matrix and the model matrix uniforms.
    glUniformMatrix4fv(glGetUniformLocation(shader, "VPMatrix"), 1, GL_FALSE, glm::value_ptr(VP));
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));

    // Bind the component’s texture (if any).
    BindTexture();

    // Finally, call the base MeshModel::Render to issue the draw call.
    MeshModel::Render(shader);
}

void MeshRenderer::OnInspectorGUI() {
    // Expose component-specific properties via ImGui.
    ImGui::DragFloat("Movement Speed", &a, 0.1f);
    ImGui::DragFloat("Mouse Sensitivity", &b, 0.01f);
}
