/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : MeshModel.h
Description : Header file for the MeshModel class, which manages the loading, updating, and rendering of a 3D mesh model.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/


#pragma once

#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <ext/matrix_transform.hpp>
#include "VertexStandard.h"

// Manages the loading, updating, and rendering of a 3D mesh model.
class MeshModel
{
public:
    // Constructor that initializes the model with position, rotation, scale, and file path.
    MeshModel(glm::vec3 _position = glm::vec3(0), glm::vec3 _rotation = glm::vec3(0), glm::vec3 _scale = glm::vec3(0), std::string _modelFilePath = "");

    // Destructor.
    ~MeshModel();

    // Updates the model with the given delta time.
    void Update(float _deltaTime);

    // Renders the model.
    virtual void Render();

    // Binds the texture for rendering.
    void BindTexture();

    // Sets the texture ID for the model.
    void SetTexture(GLuint _textureID);

    // Sets the shader program for the model.
    void SetShader(GLuint _shader);

    // Sets the position of the model.
    void SetPosition(glm::vec3 _newPos);

    // Gets the position of the model.
    glm::vec3 GetPosition();

    // Calculates the model matrix based on position, rotation, and scale.
    glm::mat4 CalculateModelMatrix();

protected:
    // Vertex Array Object and rendering details
    GLuint VAO;
    GLuint m_drawCount;
    int m_drawType;

    // Shader program ID
    GLuint m_shader;

    // Texture ID
    GLuint m_texture;

    // Model transformation data
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_modelMatrix;
};
