#pragma once
#include "Component.h"  
#include <glm/glm.hpp>

class CameraMovement : public Component{
public:
    CameraMovement();

    virtual void Start() override;
    virtual void Update(float dt) override;
    virtual void OnInspectorGUI() override;

private:
    float movementSpeed;
    float mouseSensitivity;
    float lastX;
    float lastY;
    float yaw;
    float pitch;
    bool  firstMouse;
    bool  capturingMouse = false;
};

