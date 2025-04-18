#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace1;
in vec4 FragPosLightSpace2;
in float Height;

uniform sampler2D grassTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;
uniform sampler2D snowTexture;
uniform sampler2D shadowMap1; 
uniform sampler2D shadowMap2; 

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// Define height thresholds
const float grassHeight = 30.0;
const float dirtHeight = 60.0;
const float stoneHeight = 90.0;
const float snowHeight = 120.0;

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, float heightAboveTerrain)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Retrieve the closest depth value from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Calculate the current depth
    float currentDepth = projCoords.z;

    // Calculate dynamic bias based on height
    float baseBias = 0.005;
    float dynamicBias = baseBias + heightAboveTerrain * 0.01; // Adjust multiplier as needed

    // Determine if the fragment is in shadow
    float shadow = currentDepth - dynamicBias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    // Texture blending based on height
    vec4 grassColor = texture(grassTexture, TexCoord);
    vec4 dirtColor = texture(dirtTexture, TexCoord);
    vec4 stoneColor = texture(stoneTexture, TexCoord);
    vec4 snowColor = texture(snowTexture, TexCoord);

    float blendGrass = 1.0 - smoothstep(grassHeight, dirtHeight, Height);
    float blendDirt = smoothstep(grassHeight, dirtHeight, Height) * (1.0 - smoothstep(dirtHeight, stoneHeight, Height));
    float blendStone = smoothstep(dirtHeight, stoneHeight, Height) * (1.0 - smoothstep(stoneHeight, snowHeight, Height));
    float blendSnow = smoothstep(stoneHeight, snowHeight, Height);

    vec4 finalColor = grassColor * blendGrass + dirtColor * blendDirt + stoneColor * blendStone + snowColor * blendSnow;

    // Lighting calculations
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(lightPos1 - FragPos);
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);   
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse = (diff1 + diff2) * lightColor * 0.5;
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32);
    vec3 specular = specularStrength * (spec1 + spec2) * lightColor * 0.5;  
    
    // Calculate shadows from both lights
    float heightAboveTerrain = FragPos.y - Height; // Assuming Height is the terrain height at FragPos
    float shadow1 = ShadowCalculation(FragPosLightSpace1, shadowMap1, heightAboveTerrain); 
    float shadow2 = ShadowCalculation(FragPosLightSpace2, shadowMap2, heightAboveTerrain); 
    float shadow = (shadow1 + shadow2); // Combine shadows
    float shadowIntensity = 0.5;

    vec3 result = (ambient + (1.0 - shadow * shadowIntensity) * (diffuse + specular)) * objectColor;

    FragColor = finalColor * vec4(result, 1.0);
}
