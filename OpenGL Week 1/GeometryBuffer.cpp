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

void GeometryBuffer::PopulateProgram(GLuint Program)
{

    // Bind textures to texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture_Position);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture_Normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Texture_AlbedoShininess);

    // Set uniform sampler locations
    GLint locPosition = glGetUniformLocation(Program, "Texture_Position");
    GLint locNormal = glGetUniformLocation(Program, "Texture_Normal");
    GLint locAlbedoSpec = glGetUniformLocation(Program, "Texture_AlbedoShininess");

    if (locPosition != -1) {
        glUniform1i(locPosition, 0); // Texture unit 0
    }
    if (locNormal != -1) {
        glUniform1i(locNormal, 1); // Texture unit 1
    }
    if (locAlbedoSpec != -1) {
        glUniform1i(locAlbedoSpec, 2); // Texture unit 2
    }


}

void GeometryBuffer::WriteDepth()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, 800, 800, 0, 0, 800, 800, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
