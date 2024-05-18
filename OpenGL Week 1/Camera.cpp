#include "Camera.h"

void Camera::MoveCamera(glm::vec3 _position)
{
	Position = _position;
}

void Camera::InitCamera(int _width, int _height, glm::vec3 _position)
{
	Width = (float)_width;
	Height = (float)_height;
	MoveCamera(_position);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Camera::Matrix(float nearPlane, float farPlane, GLuint shaderID, const char* uniform)
{

    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
	//view = glm::lookAt(Position, Position + Orientation, Up);
	view = glm::lookAt(Position, glm::vec3(0), Up);

    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(45.0f), (Width / Height), nearPlane, farPlane);

    // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));

}
