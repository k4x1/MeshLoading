#include "Camera.h"
#include "GameObject.h"
void Camera::InitCamera(float width, float height) {
    m_width = width;
    m_height = height;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

glm::mat4 Camera::GetViewMatrix() {
    glm::vec3 pos       = owner->transform.position;
    glm::vec3 direction = owner->transform.GetForward();
    glm::vec3 up        = owner->transform.GetUp();
    m_view = glm::lookAt(pos, pos + direction, -up);
    return m_view;
}


glm::mat4 Camera::GetProjectionMatrix() {
    m_projection = glm::perspective(glm::radians(m_FOV), m_width / m_height, 0.01f, 10000.0f);
    return m_projection;
}

void Camera::Matrix(float nearPlane, float farPlane, GLuint shaderID, const char* uniform) {
    m_view = GetViewMatrix();
    m_projection = glm::perspective(glm::radians(m_FOV), m_width / m_height, nearPlane, farPlane);
    if (uniform != nullptr && shaderID != 0) {
        glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(m_projection * m_view));
    }
}
