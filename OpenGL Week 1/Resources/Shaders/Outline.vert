#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

uniform mat4 ModelMat;
uniform mat4 VPMatrix;
uniform float OutlineScale = 1.05; // Scale factor for the outline

void main()
{
    // Scale the model matrix to create the outline effect
    mat4 scaledModelMat = ModelMat * OutlineScale;
    gl_Position = VPMatrix * scaledModelMat * vec4(Position, 1.0);
}
