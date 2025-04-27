

#pragma once
#include <iostream>
#include "EnginePluginAPI.h"

class ENGINE_API Texture
{
public:
    Texture() = default;

    ~Texture();

    unsigned int GetId() const;

    void InitTexture(const char* _filePath);

private:

    unsigned int m_textureID = 0;
};