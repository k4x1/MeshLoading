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
	//view = glm::lookAt(glm::vec3(0,0,10.0f), glm::vec3(0,0,0), Up);
	// Adds perspective to the scene	
	//projection = glm::ortho(-Width/2,Width/2,-Height/2,Height/2, nearPlane, farPlane);
	projection = glm::perspective(glm::radians(45.0f) ,(Width/Height), nearPlane, farPlane);
 
	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}
