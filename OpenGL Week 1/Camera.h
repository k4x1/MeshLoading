#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glfw3.h>
#include <iostream>
#include "ShaderLoader.h"

class Camera
{
public:
    // Stores the main vectors of the camera
    glm::vec3 m_position;
    glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 m_view = glm::mat4(1.0f);
    // Prevents the camera from jumping around when first clicking left click
    bool m_firstClick = true;

    // Stores the width and height of the window
    float m_width;
    float m_height;


    // Adjust the speed of the camera and its sensitivity when looking around
    float m_speed = 50.0f;
    float m_sensitivity = 10.0f;

    // Yaw and pitch angles for camera orientation
    float m_yaw = -90.0f; // Initialized to -90.0 degrees to look forward
    float m_pitch = 0.0f;
    float m_radius = 100.0f;

    void MoveCamera(glm::vec3 _position);
    // Camera constructor to set up initial values
    void InitCamera(int _width, int _height, glm::vec3 _position);

    // Updates and exports the camera matrix to the Vertex Shader
    void Matrix(float _nearPlane, float _farPlane, GLuint _shaderID, const char* _uniform);
};

#endif