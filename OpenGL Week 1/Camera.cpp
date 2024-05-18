#include "Camera.h"

void Camera::MoveCamera(glm::vec3 _position)
{
	m_position = _position;
}

void Camera::InitCamera(int _width, int _height, glm::vec3 _position)
{
	m_width = (float)_width;
	m_height = (float)_height;
	MoveCamera(_position);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Camera::Matrix(float _nearPlane, float _farPlane, GLuint _shaderID, const char* _uniform)
{

    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
	//view = glm::lookAt(Position, Position + Orientation, Up);
	m_view = glm::lookAt(m_position, glm::vec3(0), m_up);

    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(45.0f), (m_width / m_height), _nearPlane, _farPlane);

    // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(_shaderID, _uniform), 1, GL_FALSE, glm::value_ptr(projection * m_view));

}
