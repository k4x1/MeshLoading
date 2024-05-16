#pragma once

#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include <ext/matrix_transform.hpp>
#include "VertexStandard.h"

class MeshModel
{
public:
    MeshModel(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale, std::string ModelFilePath);
    ~MeshModel();

    void Update(float DeltaTime);
    void Render();
    void LoadModel();
    void InitTexture(std::string _filePath);
    void BindTexture();
    void SetShader(GLuint _shader);
    glm::mat4 CalculateModelMatrix();
protected:
    //vao and rendering
    GLuint VAO;
    GLuint DrawCount;
    int DrawType;

    //shader
    GLuint m_shader;

    //texture and data
    GLuint m_texture;
    unsigned char* imageData;

    //other
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_ModelMatrix;
};
