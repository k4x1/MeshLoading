
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>
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

//void Texture::SaveTextureToFile(const char* _filePath)
//{
//    glBindTexture(GL_TEXTURE_2D, m_textureID);
//
//    // Get the texture width, height, and format
//    int width, height;
//    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
//    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
//
//    // Assuming the texture format is GL_RGBA
//    int channels = 4;
//    GLenum format = GL_RGBA;
//
//    // Allocate memory to store the texture data
//    unsigned char* data = new unsigned char[width * height * channels];
//
//    // Read the texture data from the GPU
//    glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data);
//
//    // Save the texture data to a file using stb_image_write
//    if (stbi_write_png(_filePath, width, height, channels, data, width * channels)) {
//        std::cout << "Texture saved successfully to " << _filePath << std::endl;
//    }
//    else {
//        std::cerr << "Failed to save texture to " << _filePath << std::endl;
//    }
//
//    // Clean up
//    delete[] data;
//    glBindTexture(GL_TEXTURE_2D, 0);
//}
