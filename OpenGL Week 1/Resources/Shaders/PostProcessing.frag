#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int effect;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    
    if (effect == 1) // Inversion
    {
        FragColor = vec4(1.0 - col, 1.0);
    }
    else if (effect == 2) // Greyscale (luminosity method)
    {
        float luminosity = 0.21 * col.r + 0.71 * col.g + 0.07 * col.b;
        FragColor = vec4(luminosity, luminosity, luminosity, 1.0);
    }
    else 
    {
        FragColor = vec4(col, 1.0);
    }
}
