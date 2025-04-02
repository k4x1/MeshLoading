#pragma once
#include "IInspectable.h"

class GameObject;

class Component : public IInspectable {
public:
    GameObject* owner = nullptr;

    virtual void Start() {}
    virtual void Update(float dt) {}
    virtual void Render(class Camera* cam) {}
    virtual void OnInspectorGUI() {}  
    virtual ~Component() {}
};
