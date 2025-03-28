#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#pragma once

#include "Component.h"
#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ShaderLoader.h"

class GameObject;

class Camera : public Component {
public:
    // Camera-specific properties
    float m_FOV = 90.0f;
    float m_width = 800.0f;
    float m_height = 800.0f;
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Cached matrices
    glm::mat4 m_view = glm::mat4(1.0f);
    glm::mat4 m_projection = glm::mat4(1.0f);

    Camera() = default;
    virtual ~Camera() = default;

    // Initialize camera settings and OpenGL state
    void InitCamera(float width, float height);

    // Returns the view matrix.
    glm::mat4 GetViewMatrix();

    // Returns the projection matrix.
    glm::mat4 GetProjectionMatrix();

    // Sets the shader uniform using the product of projection and view matrices.
    void Matrix(float nearPlane, float farPlane, GLuint shaderID = 0, const char* uniform = nullptr);
};

#endif // CAMERA_CLASS_H
