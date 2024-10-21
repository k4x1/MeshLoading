#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 Velocity;
layout (location = 2) in vec4 Color;

out vec4 ParticleColor;
out float ParticleLife;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(Position.xyz, 1.0f);
    ParticleColor = Color;
    ParticleLife = Position.w;
}
