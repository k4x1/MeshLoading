#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

uniform mat4 ModelMat;
uniform mat4 VPMatrix;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;

void main()
{
	FragTexCoords = TexCoords;	
	FragNormal = mat3(transpose(inverse(ModelMat))) * Normal;
	gl_Position = VPMatrix * ModelMat *vec4(Position, 1.0f);
	FragPos = vec3(ModelMat * vec4(Position,1.0f));
}