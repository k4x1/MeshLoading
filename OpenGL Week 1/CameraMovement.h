#pragma once
#include "Component.h"  
#include <glm.hpp>

class CameraMovement : public Component {
public:
    CameraMovement();

    virtual void Start() override;

    virtual void Update(float dt) override;

private:
    float movementSpeed;
    float mouseSensitivity;
    bool firstMouse;
    float lastX;
    float lastY;

};
