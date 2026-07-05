#include "PreviewRenderTarget.h"

#include "../../Utils/Debug.h"

PreviewRenderTarget::~PreviewRenderTarget()
{
    Destroy();
}

void PreviewRenderTarget::EnsureSize(int newWidth, int newHeight)
{
    if (frameBuffer != 0 &&
        width == newWidth &&
        height == newHeight)
    {
        return;
    }

    Destroy();

    width = newWidth;
    height = newHeight;

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorTexture,
        0
    );

    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        width,
        height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        depthRenderBuffer
    );

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        DEBUG_ERR("Preview framebuffer is incomplete.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PreviewRenderTarget::Bind()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

void PreviewRenderTarget::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);

    glViewport(
        previousViewport[0],
        previousViewport[1],
        previousViewport[2],
        previousViewport[3]
    );
}

GLuint PreviewRenderTarget::GetColorTexture() const
{
    return colorTexture;
}

void PreviewRenderTarget::Destroy()
{
    if (depthRenderBuffer != 0)
    {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer = 0;
    }

    if (colorTexture != 0)
    {
        glDeleteTextures(1, &colorTexture);
        colorTexture = 0;
    }

    if (frameBuffer != 0)
    {
        glDeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }

    width = 0;
    height = 0;
}