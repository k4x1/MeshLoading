#version 460 core

layout (location = 0) in vec3 Position;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = Position;
    vec4 pos = projection * view * vec4(Position, 1.0);
    gl_Position = pos.xyww; // Perspective division to set z-component to 1.0
}
