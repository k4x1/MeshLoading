#version 460 core

in vec3 FragColor;
out vec4 FinalColor;

uniform float CurrentTime;

// pseudo random function https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl (pretty fun)
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // calculate pulsating factor based on time
    float pulseFactor = abs(sin(CurrentTime));

    float pseudoFrameRate = 0.3; // lower the values the slower it is
    float timeForColorChange = floor(CurrentTime * pseudoFrameRate);

    // generate a random color based on time
    vec3 randomColor = vec3(rand(vec2(timeForColorChange, timeForColorChange)),
                              rand(vec2(timeForColorChange + 100.0, timeForColorChange + 100.0)),
                              rand(vec2(timeForColorChange + 200.0, timeForColorChange + 200.0)));

    // interpolate between original color and random color using pulseFactor
    vec3 finalColor = mix(FragColor, randomColor, pulseFactor);

    FinalColor = vec4(finalColor, 1.0);
}

