/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : Skybox.h
Description : Header file for the Skybox class, which manages the loading, updating, and rendering of a skybox.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include "MeshModel.h"
#include <vector>
#include <string>
#include <glm.hpp>

class Skybox : public MeshModel {
public:
    Skybox(const std::string& objFilePath, const std::vector<std::string>& faces);
    void Render(const glm::mat4& view, const glm::mat4& projection);
    GLuint GetCubemapTexture();

private:
    GLuint cubemapTexture;
    GLuint shaderProgram;

    void loadCubemap(const std::vector<std::string>& faces);
    void setupShader();
};

#endif
