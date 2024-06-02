/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : Skybox.cpp
Description : Implementation file for the Skybox class, which manages the loading, updating, and rendering of a skybox.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/

#include "Skybox.h"
#include "stb_image.h"
#include "ShaderLoader.h"

Skybox::Skybox(Camera* camera, const std::vector<std::string>& faces)
    : MeshModel(glm::vec3(0), glm::vec3(0), glm::vec3(1), ""), camera(camera) {
    loadCubemap(faces);
    SetShader(ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag"));
}

Skybox::~Skybox() {
    glDeleteTextures(1, &textureID);
}

void Skybox::loadCubemap(const std::vector<std::string>& faces) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::Render() {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(m_shader);
    camera->Matrix(0.01f, 1000.0f, m_shader, "VPMatrix");
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
