#include "Camera.h"
#include "ComponentFactory.h"


void Camera::InitCamera(float width, float height) {
    m_width = width;
    m_height = height;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

glm::mat4 Camera::GetViewMatrix() {
    glm::mat4 world = owner->GetWorldMatrix();

    glm::vec3 pos = glm::vec3(world[3]);

    glm::vec3 forward = glm::normalize(glm::vec3(world * glm::vec4(0, 0, -1, 0)));
    glm::vec3 up = glm::normalize(glm::vec3(world * glm::vec4(0, 1, 0, 0)));

    m_view = glm::lookAt(pos, pos + forward, -up);
    return m_view;
}

glm::mat4 Camera::GetProjectionMatrix() {
    m_projection = glm::perspective(glm::radians(m_FOV), m_width / m_height, 0.01f, 10000.0f);
    return m_projection;
}

void Camera::Matrix(float nearPlane, float farPlane, GLuint shaderID, const char* uniform) {
    m_view = GetViewMatrix();
    m_projection = glm::perspective(glm::radians(m_FOV), m_width / m_height, nearPlane, farPlane);
    if (shaderID && uniform) {
        glm::mat4 VP = m_projection * m_view;
        glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(VP));
    }
}

