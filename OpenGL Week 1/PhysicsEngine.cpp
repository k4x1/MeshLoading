#include "PhysicsEngine.h"

PhysicsEngine& PhysicsEngine::Instance() {
    static PhysicsEngine inst;
    return inst;
}

PhysicsEngine::PhysicsEngine() {
    m_world = m_common.createPhysicsWorld();
    m_world->setGravity(reactphysics3d::Vector3(0, -0.09f, 0));
}

PhysicsEngine::~PhysicsEngine() {
    if (m_world) m_common.destroyPhysicsWorld(m_world);
}

reactphysics3d::PhysicsCommon& PhysicsEngine::GetCommon() {
    return m_common;
}

reactphysics3d::PhysicsWorld* PhysicsEngine::GetWorld() {
    return m_world;
}
void PhysicsEngine::Update(float dt) {
    m_world->update(dt);
}