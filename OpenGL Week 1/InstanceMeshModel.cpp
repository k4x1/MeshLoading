/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : InstanceMeshModel.cpp
Description : Implementation file for the InstanceMeshModel class, which extends MeshModel to support instanced rendering.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#include "InstanceMeshModel.h"

// Constructor for InstanceMeshModel
InstanceMeshModel::InstanceMeshModel(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, std::string _modelFilePath)
    : MeshModel(_position, _rotation, _scale, _modelFilePath), m_countInstanced(0)
{

}

// Destructor for InstanceMeshModel
InstanceMeshModel::~InstanceMeshModel()
{
    // Delete the instance VBO
    glDeleteBuffers(1, &m_instanceVBO);
}

// Render function for instanced rendering
void InstanceMeshModel::Render()
{
    glUniform1i(glGetUniformLocation(m_shader, "Texture0"), 0);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(m_drawType, 0, m_drawCount, m_countInstanced);
    glBindVertexArray(0);
}

// Add an instance with specific position, rotation, and scale
void InstanceMeshModel::AddInstance(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
{
    // Calculate the translation, rotation, and scale matrices
    glm::mat4 TranslationMat = glm::translate(glm::identity<glm::mat4>(), _position);
    glm::mat4 RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 ScaleMat = glm::scale(glm::identity<glm::mat4>(), _scale);

    // Combine the matrices to form the model-view-projection matrix
    m_instancedMVPs.push_back(TranslationMat * RotationMat * ScaleMat);
    m_countInstanced++;
}

void InstanceMeshModel::initBuffer()
{
    // Initialize instance VBO
    glBindVertexArray(VAO);
    glGenBuffers(1, &m_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instancedMVPs.size() * sizeof(glm::mat4), m_instancedMVPs.data(), GL_STATIC_DRAW); // needs to happen once

    // Set instance attribute pointers for the matrices
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }
    glBindVertexArray(0);
}
