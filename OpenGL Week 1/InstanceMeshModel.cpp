#include "InstanceMeshModel.h"

InstanceMeshModel::InstanceMeshModel(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale, std::string ModelFilePath)
    : MeshModel(Position, Rotation, Scale, ModelFilePath), m_countInstanced(0)
{
    // Initialize instance VBO
    glGenBuffers(1, &instanceVBO);
}

InstanceMeshModel::~InstanceMeshModel()
{
    glDeleteBuffers(1, &instanceVBO);
}

void InstanceMeshModel::LoadModel()
{
    MeshModel::LoadModel();
}

void InstanceMeshModel::Render()
{
    glBindVertexArray(VAO);

    // Update instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instancedMVPs.size() * sizeof(glm::mat4), m_instancedMVPs.data(), GL_STATIC_DRAW);

    // Set instance attribute pointers
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);
    }

    glUniform1i(glGetUniformLocation(m_shader, "Texture0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ModelMat"), 1, GL_FALSE, &m_ModelMatrix[0][0]);

    glDrawArraysInstanced(m_drawType, 0, m_drawCount, m_countInstanced);

    glBindVertexArray(0);
}

void InstanceMeshModel::AddInstance(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
{
    glm::mat4 TranslationMat = glm::translate(glm::identity<glm::mat4>(), _position);
    glm::mat4 RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 ScaleMat = glm::scale(glm::identity<glm::mat4>(), _scale);

    m_instancedMVPs.push_back(TranslationMat * RotationMat * ScaleMat);
    m_countInstanced++;
}
