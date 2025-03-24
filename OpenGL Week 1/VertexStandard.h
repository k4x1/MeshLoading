


#pragma once
#include <glm.hpp>
struct VertexStandard
{
public:
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;

    VertexStandard()
    {
        position = glm::vec3(0.0f);
        texcoord = glm::vec2(0.0f);
        normal = glm::vec3(0.0f);
    }

    VertexStandard(glm::vec3 _pos, glm::vec2 _texc, glm::vec3 _nor)
    {
        position = _pos;
        texcoord = _texc;
        normal = _nor;
    }
};