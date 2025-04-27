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
    auto& T = owner->transform;
    glm::vec3 pos = T.position;
    glm::vec3 fwd = T.GetForward();
    glm::vec3 up = T.GetUp();
    m_view = glm::lookAt(pos, pos + fwd, -up);
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

