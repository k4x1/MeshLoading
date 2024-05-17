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
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::mat4(1.0f);
    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    // Stores the width and height of the window
    float Width;
    float Height;

    // Adjust the speed of the camera and its sensitivity when looking around
    float speed = 10.0f;
    float sensitivity = 100.0f;

    // Yaw and pitch angles for camera orientation
    float yaw = -90.0f; // Initialized to -90.0 degrees to look forward
    float pitch = 0.0f;

    void MoveCamera(glm::vec3 _position);
    // Camera constructor to set up initial values
    void InitCamera(int _width, int _height, glm::vec3 _position);

    // Updates and exports the camera matrix to the Vertex Shader
    void Matrix(float nearPlane, float farPlane, GLuint shaderID, const char* uniform);
};

#endif