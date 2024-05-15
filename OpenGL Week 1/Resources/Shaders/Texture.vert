#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoords;

uniform mat4 ModelMat;
uniform mat4 CameraMatrix;

out vec3 FragColor;
out vec2 FragTexCoords;

void main()
{
	gl_Position = CameraMatrix * ModelMat *vec4(Position, 1.0f);

	FragColor = Color;
	FragTexCoords = TexCoords;
}