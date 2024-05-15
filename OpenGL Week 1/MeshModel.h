#pragma once

#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "VertexStandard.h"

class MeshModel
{
public:
    MeshModel(std::string FilePath);
    ~MeshModel();

    void Update(float DeltaTime);
    void Render();
    void LoadModel();
    void InitTexture(std::string _filePath);
    void SetTexture();
    void SetShader(GLuint _shader);
    void DefineModelMatrix(glm::vec3 _pos, float _rot, glm::vec3 _scl);

protected:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint texture;
    GLuint shaderProgram;
    GLuint DrawCount;
    glm::mat4 TranslationMat;
    glm::mat4 RotationMat;
    glm::mat4 ScaleMat;
    int DrawType;
    glm::mat4 ModelMat;
    unsigned char* imageData;
    int imageWidth;
    int imageHeight;
    int imageComponents;
};
