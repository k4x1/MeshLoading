#pragma once
#include <reactphysics3d/reactphysics3d.h>

class PhysicsEngine {
public:
    static PhysicsEngine& Instance();
    reactphysics3d::PhysicsCommon& GetCommon();
    reactphysics3d::PhysicsWorld* GetWorld();

    void Update(float dt);


private:
    PhysicsEngine();
    ~PhysicsEngine();
    PhysicsEngine(const PhysicsEngine&) = delete;
    void operator=(const PhysicsEngine&) = delete;

    reactphysics3d::PhysicsCommon    m_common;
    reactphysics3d::PhysicsWorld* m_world = nullptr;
};
