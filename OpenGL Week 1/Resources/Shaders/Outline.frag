#version 460 core

out vec4 FragColor;

uniform vec4 OutlineColor = vec4(1.0, 0.0, 0.0, 1.0); // Red outline color

void main()
{
    FragColor = OutlineColor;
}
