#pragma once
#include <iostream>

class Texture
{
private:
	unsigned int m_textureID;

public:
	

	Texture();
	~Texture();

	unsigned int GetId();
	void InitTexture(const char* _filePath);
};

