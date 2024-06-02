#pragma once
#include <glm.hpp>
struct Light {
    glm::vec3 color;
    float specularStrength;
};
struct PointLight : public Light {
    glm::vec3 position;
};

