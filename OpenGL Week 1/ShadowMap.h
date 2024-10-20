#pragma once
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
class ShadowMap {
public:
    ShadowMap(unsigned int width = 4096, unsigned int height = 4096);
    ~ShadowMap();

    void Bind();
    void Unbind();
    void BindTexture(GLenum TextureUnit = GL_TEXTURE0);
    GLuint GetDepthMapID() const { return m_DepthMapID; }

    glm::mat4 GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }
    void UpdateLightSpaceMatrix(const glm::vec3& lightPos, const glm::vec3& targetPos, const glm::vec3& up);

private:
    GLuint m_FBO;
    GLuint m_DepthMapID;
    unsigned int m_Width;
    unsigned int m_Height;
    glm::mat4 m_LightSpaceMatrix;

    void Initialize();
};