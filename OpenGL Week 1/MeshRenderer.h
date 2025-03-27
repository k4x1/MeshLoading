#pragma once
#include "GameObject.h"
#include "MeshModel.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

class MeshRenderer : public Component {
public:
    MeshRenderer(const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::string& modelFilePath,
        GLuint defaultShader,
        GLuint defaultTexture)
        : shader(defaultShader), texture(defaultTexture)
    {
        mesh = new MeshModel(position, rotation, scale, modelFilePath);
    }

    virtual ~MeshRenderer() {
        delete mesh;
    }

    virtual void Start() override {
        // Initialization if needed.
    }

    virtual void Update(float deltaTime) override {
        mesh->Update(deltaTime);
    }

    // Normal render uses the default shader.
    virtual void Render(Camera* cam) override {
        glm::mat4 worldMatrix = owner ? owner->GetWorldMatrix() : mesh->CalculateModelMatrix();
        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMat"), 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader, "ProjMat"), 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "Texture0"), 1);
        mesh->Render(shader);
    }

   /* void RenderWithShader( Camera* cam, GLuint _shader = NULL) {
        if (_shader == NULL)
        {
            _shader = shader;
        }
        glm::mat4 worldMatrix = owner ? owner->GetWorldMatrix() : mesh->CalculateModelMatrix();
        glUseProgram(_shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMat"), 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader, "ProjMat"), 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "Texture0"), 1);
        mesh->Render(_shader);
    }*/

    void SetShader(GLuint newShader) { shader = newShader; }
    void SetTexture(GLuint newTex) { texture = newTex; }

private:
    MeshModel* mesh;
    GLuint shader;
    GLuint texture;
};
