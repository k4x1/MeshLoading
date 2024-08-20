#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 VPMatrix;
uniform float OutlineScale = 1.05; // Scale factor for the outline

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;

void main()
{
    mat4 scaledInstanceMatrix = instanceMatrix;
    scaledInstanceMatrix[0][0] *= OutlineScale;
    scaledInstanceMatrix[1][1] *= OutlineScale;
    scaledInstanceMatrix[2][2] *= OutlineScale;

    FragTexCoords = TexCoords;
    FragNormal = mat3(transpose(inverse(scaledInstanceMatrix))) * Normal;
    gl_Position = VPMatrix * scaledInstanceMatrix * vec4(Position, 1.0);
    FragPos = vec3(scaledInstanceMatrix * vec4(Position, 1.0f));
}
