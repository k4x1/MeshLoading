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
    MeshModel(glm::vec3 _position = glm::vec3(0), glm::vec3 _rotation = glm::vec3(0), glm::vec3 _scale = glm::vec3(0), std::string _modelFilePath = "");
    ~MeshModel();

    void Update(float _deltaTime);
    virtual void Render();
    virtual void LoadModel();
    void BindTexture();
    void SetTexture(GLuint _textureID);
    void SetShader(GLuint _shader);
    void SetPosition(glm::vec3 _newPos);

    glm::vec3 GetPosition();
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
   // unsigned char* imageData;

    //other
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_modelMatrix;
};
