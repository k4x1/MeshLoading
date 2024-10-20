#version 460 core

layout (location = 0) in vec3 gPosition;
layout (location = 1) in vec2 gTexCoords;
layout (location = 2) in vec3 gNormal;

uniform mat4 ModelMat;
uniform mat4 VPMatrix;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;

void main()
{
	FragTexCoords = gTexCoords;	
	FragNormal = mat3(transpose(inverse(ModelMat))) * gNormal;
	gl_Position = VPMatrix * ModelMat *vec4(gPosition, 1.0f);
	FragPos = vec3(ModelMat * vec4(gPosition,1.0f));
}