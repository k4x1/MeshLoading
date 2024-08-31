/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : Texture.h
Description : Header file for the Texture class, which manages texture loading and binding for OpenGL.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/


#pragma once
#include <iostream>

class Texture
{
private:
    unsigned int m_textureID; // OpenGL texture ID

public:
    // Default constructor.
    Texture();

    // Destructor.
    ~Texture();

    // Gets the OpenGL texture ID.
    unsigned int GetId();

    // Initializes the texture from a file.
    void InitTexture(const char* _filePath);
    void SaveTextureToFile(const char* _filePath);
};