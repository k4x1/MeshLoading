#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 CameraPos;
uniform float AmbientStrength;
uniform vec3 AmbientColor;

#define MAX_POINT_LIGHTS 10

struct PointLight {
    vec3 position;
    vec3 color;
    float specularStrength;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationExponent;
};

uniform PointLight PointLightArray[MAX_POINT_LIGHTS];
uniform int PointLightCount;

vec3 CalculateLightPoint(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.specularStrength * spec * light.color;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuationConstant + light.attenuationLinear * distance + light.attenuationExponent * (distance * distance));

    return (diffuse + specular) * attenuation;
}

void main()
{
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 viewDir = normalize(CameraPos - fragPos);

    vec3 ambient = AmbientStrength * AmbientColor * albedo;

    vec3 lighting = ambient;
    for (int i = 0; i < PointLightCount; ++i)
    {
        lighting += CalculateLightPoint(PointLightArray[i], fragPos, normal, viewDir);
    }

    FragColor = vec4(lighting, 1.0);
}
