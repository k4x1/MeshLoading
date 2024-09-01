#version 460 core

out vec4 FinalColor;

in vec2 TexCoord;

uniform sampler2D texture1;

vec4 applyGradient(float value) {
    if (value < 0.25) {
        return mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), value * 2.0);
    } 
    else if (value < 0.5) {
        return mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), (value - 0.25) * 4.0);
    }
    else if (value < 0.75) {
        return mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0), (value - 0.5) * 4.0);
    }
    else {
        return mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), (value - 0.75) * 4.0);
    }
}

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    float grayValue = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
    FinalColor = applyGradient(grayValue);
}
