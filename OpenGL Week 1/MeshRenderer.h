
#include "GameObject.h"
#include "MeshModel.h"
#include "Camera.h"
#include <gtc/type_ptr.hpp>

class MeshRenderer : public Component, public MeshModel {
public:
    MeshRenderer(const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::string& modelFilePath)
        : MeshModel(position, rotation, scale, modelFilePath) {}

    virtual void Update(float deltaTime) override {
        MeshModel::Update(deltaTime);
    }

    virtual void Render(Camera* cam) override {
        glm::mat4 worldMatrix = owner ? owner->GetWorldMatrix() : CalculateModelMatrix();

        glUseProgram(GetShader());
        glUniformMatrix4fv(glGetUniformLocation(GetShader(), "ModelMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
        MeshModel::Render(GetShader());
    }

};

