#version 460 core
#define MAX_POINT_LIGHTS 10

struct PointLight {
    vec3 position;
    vec3 color;
    float specularStrength;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationExponent;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float specularStrength;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float specularStrength;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationExponent;
    float innerCutoff; 
    float outerCutoff;
};

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPos;

uniform sampler2D Texture0;
uniform samplerCube skybox;

uniform vec3 CameraPos;
uniform float LightSpecularStrength = 1.0f;
uniform float ObjectShininess = 32.0f;

uniform float AmbientStrength = 0.15f;
uniform vec3 AmbientColor = vec3(1.0f, 1.0f, 1.0f);

uniform PointLight PointLightArray[MAX_POINT_LIGHTS];
uniform uint PointLightCount;

uniform DirectionalLight DirLight;
uniform SpotLight SptLight;

out vec4 FinalColor;

vec3 CalculateLightPoint(uint index) {
    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(FragPos - PointLightArray[index].position);

    float DiffuseStrength = max(dot(Normal, -LightDir), 0.0f);
    vec3 Diffuse = DiffuseStrength * PointLightArray[index].color;

    vec3 ReverseViewDir = normalize(CameraPos - FragPos);
    vec3 HalfwayVector = normalize(-LightDir + ReverseViewDir);

    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 Specular = PointLightArray[index].specularStrength * SpecularReflectivity * PointLightArray[index].color;

    vec3 Light = Diffuse + Specular;

    float Distance = length(PointLightArray[index].position - FragPos);
    float Attenuation = PointLightArray[index].attenuationConstant + 
                        (PointLightArray[index].attenuationLinear * Distance) + 
                        (PointLightArray[index].attenuationExponent * pow(Distance, 2));
    Light /= Attenuation;
    return Light;
}

vec3 CalculateLightSpot() {
    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(SptLight.position - FragPos);

    float Theta = dot(LightDir, normalize(-SptLight.direction));
    float Epsilon = SptLight.innerCutoff - SptLight.outerCutoff;
    float Intensity = clamp((Theta - SptLight.outerCutoff) / Epsilon, 0.0, 1.0);

    float DiffuseStrength = max(dot(Normal, -LightDir), 0.0f);
    vec3 Diffuse = DiffuseStrength * SptLight.color;

    vec3 ViewDir = normalize(FragPos - CameraPos);
    vec3 HalfwayVector = normalize(LightDir - ViewDir);
    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 Specular = SptLight.specularStrength * SpecularReflectivity * SptLight.color;

    Diffuse *= Intensity;
    Specular *= Intensity;

    vec3 Light = Diffuse + Specular;

    float Distance = length(SptLight.position - FragPos);
    float Attenuation = SptLight.attenuationConstant + 
                        (SptLight.attenuationLinear * Distance) + 
                        (SptLight.attenuationExponent * (Distance * Distance));
    Light /= Attenuation;

    return Light;
}

vec3 CalculateLightDirection() {
    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(-DirLight.direction);

    float DiffuseStrength = max(dot(Normal, LightDir), 0.0f);
    vec3 Diffuse = DiffuseStrength * DirLight.color;

    vec3 ReverseViewDir = normalize(CameraPos - FragPos);
    vec3 HalfwayVector = normalize(LightDir + ReverseViewDir);

    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 Specular = DirLight.specularStrength * SpecularReflectivity * DirLight.color;

    return Specular + Diffuse;
}

void main() {
    vec3 Ambient = AmbientStrength * AmbientColor;

    vec3 TotalLightOutput = vec3(0.0f);
    for (uint i = 0; i < PointLightCount; ++i) {
        TotalLightOutput += CalculateLightPoint(i);
    }
    TotalLightOutput += CalculateLightDirection();
    TotalLightOutput += CalculateLightSpot();

    vec4 Light = vec4(Ambient + TotalLightOutput, 1.0f);

    vec3 Normal = normalize(FragNormal);
    vec3 ViewDir = normalize(FragPos - CameraPos);
    vec3 ReflectDir = reflect(ViewDir, Normal);

    vec4 BaseTexture = texture(Texture0, FragTexCoords);
    vec4 ReflectionTexture = texture(skybox, ReflectDir);

    FinalColor = Light * mix(BaseTexture, ReflectionTexture, 0.5f);
}
