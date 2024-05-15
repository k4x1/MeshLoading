#pragma once

#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include <glfw3.h>
#include "VertexStandard.h"
class MeshModel
{
public:
    MeshModel(std::string FilePath);
    ~MeshModel();

    virtual void Update(float DeltaTime);
    virtual void Render();
    virtual void LoadModel();
    //virtual void InitTexture(std::string _filePath);
   // virtual void SetTexture();
    //virtual void setShader(GLuint _shader);
   // virtual void defineModelMatrix(glm::vec3 _pos, float _rot, glm::vec3 _scl);
protected:
    GLuint VAO;
    GLuint DrawCount;
    int DrawType;
};
