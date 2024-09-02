#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int effect;

const float offset = 1.0 / 300.0;  

vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right    
);

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    
    if (effect == 1) {
        // Inversion
        FragColor = vec4(1.0 - col, 1.0);
    }
    else if (effect == 2) {
        // Grayscale
        float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else if (effect == 3) {
        // Edge detection
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
        
        FragColor = vec4(col, 1.0);
    }
    else if (effect == 4) {
        // Custom effect (e.g., sepia)
        vec3 sepiaColor = vec3(
            dot(col, vec3(0.393, 0.769, 0.189)),
            dot(col, vec3(0.349, 0.686, 0.168)),
            dot(col, vec3(0.272, 0.534, 0.131))
        );
        FragColor = vec4(sepiaColor, 1.0);
    }
    else {
        // No effect
        FragColor = vec4(col, 1.0);
    }
}
