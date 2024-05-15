#pragma once
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glfw3.h>
#include <iostream>
#include "ShaderLoader.h"

enum Modes {
	MIRRORED,
	ANIMATED
};

class VertexObject
{
	private:
		GLfloat Vertices;
		GLuint VBO_ID;
		GLuint VAO_ID;
		GLuint Shader_ID;	
		GLuint EB_ID;

		//modelMatrix stuf
		glm::mat4 TranslationMat;
		glm::mat4 RotationMat;
		glm::mat4 ScaleMat;
		///////////////////
		// 
		//texture stuff
		GLuint texture;
		unsigned char* imageData;
		int imageWidth = 100;
		int imageHeight = 100;
		int imageComponents;
		/////////////////
	public:
		glm::mat4 ModelMat;
		GLuint getVAO();
		
		void draw(GLuint _indicesCount);

		void InitTexture(const char* _filePath);
		void setTexture(Modes _mode);

		void setShader(GLuint _Shader);
		void setEBO(GLuint* _IndicesList, GLsizei _indicesSize);
		void setVBO(GLfloat* _vertices, GLsizei _VertSize);
		void setVAO(GLuint _VAO = NULL);
		void defineModelMatrix(glm::vec3 QuadPosition, float QuadRotation, glm::vec3 QuadScale);

};

