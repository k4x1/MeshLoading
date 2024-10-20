#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace1; 
out vec4 FragPosLightSpace2; 
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix1; 
uniform mat4 lightSpaceMatrix2; 

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    Height = aPos.y;  
    
    // Calculate the fragment position in light space for both lights
    FragPosLightSpace1 = lightSpaceMatrix1 * vec4(FragPos, 1.0); 
    FragPosLightSpace2 = lightSpaceMatrix2 * vec4(FragPos, 1.0); 
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
