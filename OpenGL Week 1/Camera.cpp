/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : Camera.cpp
Description : Implementation file for the Camera class, which manages the camera's position, orientation, and view/projection matrices.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#include "Camera.h"

// Moves the camera to a new position
void Camera::MoveCamera(glm::vec3 _position)
{
    m_position = _position;
}

// Initializes the camera with given width, height, and position
void Camera::InitCamera(int _width, int _height, glm::vec3 _position)
{
    m_width = static_cast<float>(_width);
    m_height = static_cast<float>(_height);
    MoveCamera(_position);

    // Enable face culling and depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

// Updates and exports the camera matrix to the vertex shader
void Camera::Matrix(float _nearPlane, float _farPlane, GLuint _shaderID , const char* _uniform )
{
    // Initialize the projection matrix
    m_projection = glm::mat4(1.0f);

    // Make the camera look in the right direction from the right position
    m_view = glm::lookAt(m_position, m_position+m_orientation, m_up);

    // Add perspective to the scene
    m_projection = glm::perspective(glm::radians(m_FOV), (m_width / m_height), _nearPlane, _farPlane);
    if(_uniform!=nullptr && _shaderID!= NULL){
    // Export the camera matrix to the vertex shader
        glUniformMatrix4fv(glGetUniformLocation(_shaderID, _uniform), 1, GL_FALSE, glm::value_ptr(m_projection * m_view));
    }
    
}

glm::mat4 Camera::GetViewMatrix() 
{

    m_view = glm::lookAt(m_position, m_position + m_orientation, m_up);
    return m_view;
}

glm::mat4 Camera::GetProjectionMatrix() 
{
    m_projection = glm::mat4(1.0f);
    m_projection = glm::perspective(glm::radians(m_FOV), (m_width / m_height), 0.01f, 1000.0f);
    return m_projection;
}
