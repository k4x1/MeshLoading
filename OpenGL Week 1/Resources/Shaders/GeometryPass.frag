#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 FragTexCoords;
in vec3 FragPos;
in vec3 FragNormal;

uniform sampler2D Texture0;
uniform float ObjectShininess;

void main() {
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(FragNormal), 1.0);
    gAlbedoSpec.rgb = texture(Texture0, FragTexCoords).rgb;
    gAlbedoSpec.a = ObjectShininess;
}
