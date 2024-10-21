#version 460 core

layout (location = 0) in vec4 Position;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(Position.xyz, 1.0f);
}
