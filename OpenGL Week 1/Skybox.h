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

#pragma once

#include "MeshModel.h"
#include "Camera.h"
#include <vector>
#include <string>

class Skybox : public MeshModel {
public:
    Skybox(Camera* camera, const std::vector<std::string>& faces);
    ~Skybox();
    void Render() override;

private:
    GLuint textureID;
    Camera* camera;
    void loadCubemap(const std::vector<std::string>& faces);
};
