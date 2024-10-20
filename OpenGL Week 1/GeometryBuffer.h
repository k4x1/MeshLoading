#pragma once
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
class GeometryBuffer
{
public:
	GeometryBuffer();
	~GeometryBuffer();

	void Bind();
	void Unbind();
	void PopulateProgram(GLuint Program);
	void WriteDepth();
private:
	GLuint FBO;
	GLuint Texture_Position;
	GLuint Texture_Normal;
	GLuint Texture_AlbedoShininess;
	GLuint Texture_Depth;
};

