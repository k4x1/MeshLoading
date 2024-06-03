#version 460 core
#define MAX_POINT_LIGHTS 4




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

uniform vec3 CameraPos;
uniform float LightSpecularStrength = 1.0f;
uniform float ObjectShininess = 32.0f;

uniform float AmbientStrength = 0.0f;
uniform vec3 AmbientColor = vec3(1.0f, 1.0f, 1.0f);


uniform vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 LightPos = vec3(-300.0f, 0.0f, 100.0f);

uniform PointLight PointLightArray[MAX_POINT_LIGHTS];
uniform uint PointLightCount;

uniform DirectionalLight DirLight;

uniform SpotLight SptLight;

out vec4 FinalColor;

vec3 CalculateLightPoint(uint index){

	vec3 Normal = normalize(FragNormal);
	vec3 LightDir = normalize(FragPos - PointLightArray[index].position);

	float DiffuseStrength = max(dot(Normal, - LightDir), 0.0f);
	vec3 Diffuse = (DiffuseStrength) * PointLightArray[index].color;

	vec3 ReverseViewDir = normalize(CameraPos - FragPos);
	vec3 HalfwayVector = normalize(-LightDir + ReverseViewDir);

	float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
	vec3 Specular = PointLightArray[index].specularStrength * SpecularReflectivity * PointLightArray[index].color;

	vec3 Light = Diffuse + Specular;

	float Distance = length(PointLightArray[index].position - FragPos);
	float Attenuation = PointLightArray[index].attenuationConstant + (PointLightArray[index].attenuationLinear * Distance) + (PointLightArray[index].attenuationExponent * pow(Distance, 2));
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
    vec3 Diffuse = (DiffuseStrength) * SptLight.color;

    vec3 ReverseViewDir = normalize(CameraPos - FragPos);
    vec3 HalfwayVector = normalize(-LightDir + ReverseViewDir);

    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 Specular = SptLight.specularStrength * SpecularReflectivity * SptLight.color;

    vec3 Light = (Diffuse + Specular) * Intensity;

    float Distance = length(SptLight.position - FragPos);
    float Attenuation = SptLight.attenuationConstant + (SptLight.attenuationLinear * Distance) + (SptLight.attenuationExponent * pow(Distance, 2));
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
void main()
{


	vec3 Ambient = AmbientStrength * AmbientColor;

	// Specular Component
	

	vec3 TotalLightOutput = vec3(0.0f);
	for (uint i = 0; i < PointLightCount; ++i)
	{
		TotalLightOutput += CalculateLightPoint(i);
	}
	//TotalLightOutput += CalculateLightDirection();
	TotalLightOutput += CalculateLightSpot();

	// Combine the lighting components
	vec4 Light = vec4(Ambient + TotalLightOutput, 1.0f);
	
	
	

    FinalColor = Light; //* texture(Texture0, FragTexCoords);
}