#version 460 core

in vec4 ParticleColor;
in float ParticleLife;

out vec4 FragColor;

void main() {
    FragColor = vec4(ParticleColor.rgb, 1.0f);
}
