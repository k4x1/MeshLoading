

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
