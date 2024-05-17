#pragma once

#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <ext/matrix_transform.hpp>
#include "VertexStandard.h"

class MeshModel
{
public:
    MeshModel(glm::vec3 Position = glm::vec3(0), glm::vec3 Rotation = glm::vec3(0), glm::vec3 Scale = glm::vec3(0), std::string ModelFilePath = "");
    ~MeshModel();

    void Update(float DeltaTime);
    virtual void Render();
    virtual void LoadModel();
    void InitTexture(const char* _filePath);
    void BindTexture();
    void SetShader(GLuint _shader);
    glm::mat4 CalculateModelMatrix();
protected:
    //vao and rendering
    GLuint VAO;
    GLuint m_drawCount;
    int m_drawType;

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
