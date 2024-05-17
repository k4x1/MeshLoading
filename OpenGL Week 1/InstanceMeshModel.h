#pragma once

#include "MeshModel.h"
#include <vector>
#include <glm.hpp>

class InstanceMeshModel : public MeshModel
{
public:
    // Constructor
    InstanceMeshModel(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale, std::string ModelFilePath);
    ~InstanceMeshModel();
    // Member variables
    GLsizei m_countInstanced;
    std::vector<glm::mat4> m_instancedMVPs;

    // Member functions
    void LoadModel() override;
    void Render() override;
    void AddInstance(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale);

private:
    GLuint instanceVBO;
};
