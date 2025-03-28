#pragma once

#include "MeshModel.h"
#include <gtc/type_ptr.hpp>
#include <string>

class GameObject;
class Component;
class Camera;

class MeshRenderer : public Component, public MeshModel {
public:
    MeshRenderer(const glm::vec3& position,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::string& modelFilePath);

    virtual void Update(float deltaTime) override;
    virtual void Render(Camera* cam) override;
};
