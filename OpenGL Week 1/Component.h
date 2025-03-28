#pragma once
class GameObject;
class Component {
public:
    GameObject* owner = nullptr;

    virtual void Start() {}
    virtual void Update(float dt) {}
    virtual void Render(class Camera* cam) {}

    virtual ~Component() {}
};
