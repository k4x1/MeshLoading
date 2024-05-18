/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : Texture.cpp
Description : Implementation file for the Texture class, which manages texture loading and binding for OpenGL.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/




#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glew.h>
#include "Texture.h"

// constructor.
Texture::Texture()
{
    m_textureID = 0;
}

// Destructor.
Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

// Gets the OpenGL texture ID.
unsigned int Texture::GetId()
{
    return m_textureID;
}

// Initializes the texture from a file.
void Texture::InitTexture(const char* _filePath)
{
    int imageWidth = 0;
    int imageHeight = 0;
    int imageComponents = 0;

    // Flip the image vertically during loading
    stbi_set_flip_vertically_on_load(true);

    // Load the image data
    unsigned char* imageData = stbi_load(_filePath, &imageWidth, &imageHeight, &imageComponents, 0);
    if (imageData == nullptr) {
        std::cerr << "Failed to load image: " << _filePath << std::endl;
        return;
    }

    // Generate and bind the texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Determine the format of the loaded image
    int format = (imageComponents == 4) ? GL_RGBA : GL_RGB;

    // Upload the texture data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image data
    stbi_image_free(imageData);
}
