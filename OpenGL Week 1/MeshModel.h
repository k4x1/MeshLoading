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
#include "Camera.h"
#include "Light.h"

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


    GLuint GetShader();

    // Sets the position of the model.
    void SetPosition(glm::vec3 _newPos);

   
    // Pass base uniforms every mesh would need
    void PassUniforms(Camera* _camRef);

    // Pass uniforms for point light
    void PassPointUniforms(Camera* _camRef, PointLight* _lightArr, unsigned int _lightCount);

    // Pass uniforms for no texture and non light dependent models
    void PassColorUniforms(float _r, float _g, float _b, float _a);

    void PassDirectionalUniforms(DirectionalLight _light);
    
    void PassSpotLightUniforms(SpotLight _spotLight);
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
    GLuint m_texture = NULL;

    // Model transformation data
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_modelMatrix;
};
