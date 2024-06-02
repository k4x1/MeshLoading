#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 VPMatrix;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;


void main()
{
    FragTexCoords = TexCoords;
    FragNormal = mat3(transpose(inverse(instanceMatrix))) * Normal;
    gl_Position = VPMatrix * instanceMatrix * vec4(Position, 1.0);
    FragPos = vec3(instanceMatrix * vec4(Position,1.0f));
}
