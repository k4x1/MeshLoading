#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glew.h>
#include <iostream>

Texture::Texture()
    : m_textureID(0)
{}

Texture::~Texture() {
    glDeleteTextures(1, &m_textureID);
}

unsigned int Texture::GetId() const {
    return m_textureID;
}

void Texture::InitTexture(const char* _filePath) {
    int width = 0, height = 0, comps = 0;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(_filePath, &width, &height, &comps, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << _filePath << std::endl;
        return;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = (comps == 4 ? GL_RGBA : GL_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
        format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
