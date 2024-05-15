#version 460 core

uniform float CurrentTime;

in vec2 FragTexCoords;

out vec4 FinalColor;

uniform sampler2D Texture0;

void main(){
    float animSpeed = 10.0f;
    float roundedTime = round(CurrentTime * animSpeed);

    vec2 newTexCoord = vec2(FragTexCoords.x + (0.125 * roundedTime), FragTexCoords.y);
    vec4 texColor = texture(Texture0, newTexCoord);
    if(texColor.a < 0.1)
        discard;
	FinalColor = texture(Texture0, newTexCoord);
}