#pragma once
#include "../ObjectSystem/Component.h"  
#include <glm.hpp>
#include "EnginePluginAPI.h"

class ENGINE_API  CameraMovement : public Component{
public:
    CameraMovement();

    virtual void Start() override;
    virtual void Update(float dt) override;
    virtual void OnInspectorGUI() override;
    void SetCanCaptureMouse(bool value);
    
    float lastX;
    float lastY;
    float yaw;
    float pitch;

private:
    float movementSpeed;
    float mouseSensitivity;

    bool  firstMouse;
    
    bool  capturingMouse = false;
    bool  canCaptureMouse = false;
};

