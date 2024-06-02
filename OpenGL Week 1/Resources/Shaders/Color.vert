#version 460 core

layout (location = 0) in vec3 Position;

uniform mat4 ModelMat;
uniform mat4 VPMatrix;

void main()
{
	
	gl_Position = VPMatrix * ModelMat *vec4(Position, 1.0f);
	
}