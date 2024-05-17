#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in mat4 instanceMatrix;

out vec2 FragTexCoords;

uniform mat4 CameraMatrix;

void main()
{
    FragTexCoords = aTexCoord;
    gl_Position = CameraMatrix * instanceMatrix * vec4(aPos, 1.0);
}
