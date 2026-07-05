#pragma once

#include "../../EnginePluginAPI.h"

#include <glew.h>

class ENGINE_API PreviewRenderTarget
{
public:
    PreviewRenderTarget() = default;
    ~PreviewRenderTarget();

    void EnsureSize(int width, int height);
    void Bind();
    void Unbind();

    GLuint GetColorTexture() const;

private:
    GLuint frameBuffer = 0;
    GLuint colorTexture = 0;
    GLuint depthRenderBuffer = 0;

    int width = 0;
    int height = 0;

    GLint previousFrameBuffer = 0;
    GLint previousViewport[4] = { 0, 0, 0, 0 };

    void Destroy();
};