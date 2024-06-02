#pragma once
#include <glm.hpp>

//light structs 

struct Light {
    glm::vec3 color;
    float specularStrength;
};

struct PointLight : public Light {
    glm::vec3 position;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationExponent;

};

struct DirectionalLight : public Light {
    glm::vec3 Direction;
};
