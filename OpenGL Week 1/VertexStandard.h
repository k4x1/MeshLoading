/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : VertexStandard.h
Description : Header file for the VertexStandard struct, which defines the standard vertex format for the application.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/



#pragma once
#include <glm.hpp>
struct VertexStandard
{
public:
    glm::vec3 position;
    glm::vec2 texcoord;

    VertexStandard()
    {
        position = glm::vec3(0.0f);
        texcoord = glm::vec2(0.0f);
    }

    VertexStandard(glm::vec3 _pos, glm::vec2 _texc)
    {
        position = _pos;
        texcoord = _texc;
    }
};