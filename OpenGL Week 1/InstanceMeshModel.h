/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : InstanceMeshModel.h
Description : Header file for the InstanceMeshModel class, which extends MeshModel to support instanced rendering.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#pragma once

#include "MeshModel.h"
#include <vector>
#include <glm.hpp>

class InstanceMeshModel : public MeshModel
{
public:
    // Constructor
    InstanceMeshModel(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, std::string _modelFilePath);
    ~InstanceMeshModel();
    // Member variables
    GLsizei m_countInstanced;
    std::vector<glm::mat4> m_instancedMVPs;

    // Member functions
    void Render() override;
    void AddInstance(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale);
    void initBuffer();

private:
    GLuint m_instanceVBO;
};
