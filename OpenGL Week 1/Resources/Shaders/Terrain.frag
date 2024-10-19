#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in float Height;

uniform sampler2D grassTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;
uniform sampler2D snowTexture;
uniform sampler2D shadowMap; 

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// Define height thresholds
const float grassHeight = 30;
const float dirtHeight = 60;
const float stoneHeight = 90;
const float snowHeight = 120;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec4 grassColor = texture(grassTexture, TexCoord);
    vec4 dirtColor = texture(dirtTexture, TexCoord);
    vec4 stoneColor = texture(stoneTexture, TexCoord);
    vec4 snowColor = texture(snowTexture, TexCoord);

    float blendGrass = 1.0 - smoothstep(grassHeight, dirtHeight, Height);
    float blendDirt = smoothstep(grassHeight, dirtHeight, Height) * (1.0 - smoothstep(dirtHeight, stoneHeight, Height));
    float blendStone = smoothstep(dirtHeight, stoneHeight, Height) * (1.0 - smoothstep(stoneHeight, snowHeight, Height));
    float blendSnow = smoothstep(stoneHeight, snowHeight, Height);

    vec4 finalColor = grassColor * blendGrass + dirtColor * blendDirt + stoneColor * blendStone + snowColor * blendSnow;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    // Calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace); 
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;

    FragColor = finalColor * vec4(result, 1.0);
}
