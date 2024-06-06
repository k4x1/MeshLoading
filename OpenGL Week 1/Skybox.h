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

#include <glew.h>
#include <vector>
#include <string>
#include <glm.hpp>

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    void Render(const glm::mat4& view, const glm::mat4& projection);
    GLuint GetCubemapTexture() const { return cubemapTexture; }

private:
    GLuint cubemapTexture;
    GLuint skyboxVAO, skyboxVBO;
    GLuint shaderProgram;

    void loadCubemap(const std::vector<std::string>& faces);
    void setupSkybox();
    void setupShader();
};

#endif
