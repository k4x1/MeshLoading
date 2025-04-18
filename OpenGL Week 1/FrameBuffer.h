#pragma once
#include <glew.h>
#include <glm.hpp>
#include <iostream>

class FrameBuffer {
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();
    
    void Initialize();
    void Bind();
    void Unbind();
    void BindTexture(GLenum TextureUnit = GL_TEXTURE0);
    GLuint GetTextureID() const { return m_TextureID; }
    int GetWidth()  const { return m_Width; }
    int GetHeight() const { return m_Height; }


private:
    GLuint m_FBO;
    GLuint m_TextureID;
    GLuint m_RBO;
    int m_Width;
    int m_Height;

};