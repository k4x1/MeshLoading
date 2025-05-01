#version 460 core
layout (location = 0) out vec4 Texture_Position;
layout (location = 1) out vec4 Texture_Normal;
layout (location = 2) out vec4 Texture_AlbedoShininess;

in vec2 FragTexCoords;
in vec3 FragPos;
in vec3 FragNormal;

uniform sampler2D Texture0;
uniform float ObjectShininess = 1.0f;


void main() {
    Texture_Position = vec4(FragPos, 1.0);
    Texture_Normal = vec4(normalize(FragNormal), 1.0);
    Texture_AlbedoShininess.rgb = texture(Texture0, FragTexCoords).rgb;
    Texture_AlbedoShininess.a = ObjectShininess;

}
