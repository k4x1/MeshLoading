#include "GeometryBuffer.h"
GeometryBuffer::GeometryBuffer()
{

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Generate and bind the position texture
    glGenTextures(1, &Texture_Position);
    glBindTexture(GL_TEXTURE_2D, Texture_Position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 800, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture_Position, 0);

    // Generate and bind the normal texture
    glGenTextures(1, &Texture_Normal);
    glBindTexture(GL_TEXTURE_2D, Texture_Normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 800, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Texture_Normal, 0);

    // Generate and bind the albedo/shininess texture
    glGenTextures(1, &Texture_AlbedoShininess);
    glBindTexture(GL_TEXTURE_2D, Texture_AlbedoShininess);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Texture_AlbedoShininess, 0);

    // Generate and bind the depth texture
    glGenTextures(1, &Texture_Depth);
    glBindTexture(GL_TEXTURE_2D, Texture_Depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 800, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture_Depth, 0);

    // Set the list of draw buffers
    GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, DrawBuffers);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    // Unbind the framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GeometryBuffer::~GeometryBuffer()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &Texture_Position);
    glDeleteTextures(1, &Texture_Normal);
    glDeleteTextures(1, &Texture_AlbedoShininess);
    glDeleteTextures(1, &Texture_Depth);
}

void GeometryBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void GeometryBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryBuffer::PopulateProgam(GLuint Program)
{
  
}
