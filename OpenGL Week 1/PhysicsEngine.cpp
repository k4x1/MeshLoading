#include "PhysicsEngine.h"
#include "GameObject.h"
#include <reactphysics3d/collision/CollisionCallback.h>
#include <reactphysics3d/collision/OverlapCallback.h>
using namespace Physics;



PhysicsEngine& PhysicsEngine::Instance() {
    static PhysicsEngine inst;
    return inst;
}



void Physics::PhysicsEngine::initializeWorld()
{
    if (m_world) {
        m_common.destroyPhysicsWorld(m_world);
    }
    reactphysics3d::PhysicsWorld::WorldSettings ws;
    ws.persistentContactDistanceThreshold = 0.001f;
    m_world = m_common.createPhysicsWorld(ws);
    m_world->setEventListener(&m_collisionListener);
    m_world->setNbIterationsPositionSolver(30);
    m_world->setNbIterationsVelocitySolver(15);
    m_world->setGravity(reactphysics3d::Vector3(0, -9.81f, 0));
    auto& debugRef = m_world->getDebugRenderer();
    using DR = reactphysics3d::DebugRenderer;
    debugRef.setIsDebugItemDisplayed(DR::DebugItem::COLLISION_SHAPE, true);
    debugRef.setIsDebugItemDisplayed(DR::DebugItem::COLLIDER_AABB, true);
    debugRef.setIsDebugItemDisplayed(DR::DebugItem::COLLISION_SHAPE_NORMAL, false);
    debugRef.setIsDebugItemDisplayed(DR::DebugItem::CONTACT_POINT, false);
    debugRef.setIsDebugItemDisplayed(DR::DebugItem::CONTACT_NORMAL, false);
}
PhysicsEngine::PhysicsEngine() {
    initializeWorld();


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

void PhysicsEngine::FixedUpdate(float fixedDt){

    m_world->update(fixedDt);
}

void PhysicsEngine::Reset() {
    m_world->setEventListener(nullptr);
    initializeWorld();
}
bool PhysicsEngine::Raycast(
    const glm::vec3& origin,
    const glm::vec3& direction,
    RaycastHit& outHit,
    float            maxDistance)
{
    glm::vec3 dirN = glm::normalize(direction);

    glm::vec3 worldEnd = origin + dirN * maxDistance;
    reactphysics3d::Vector3 rpOrigin(origin.x, origin.y, origin.z);
    reactphysics3d::Vector3 rpEnd(worldEnd.x, worldEnd.y, worldEnd.z);

    reactphysics3d::Ray rpRay(rpOrigin, rpEnd);

    class RaycastCallbackRp : public reactphysics3d::RaycastCallback {
    public:
        RaycastCallbackRp() = default;
        virtual reactphysics3d::decimal notifyRaycastHit(
            const reactphysics3d::RaycastInfo& info) override
        {
            if (info.hitFraction <= 1e-4f) {
                return -1.0f;  
            }
            m_hasHit = true;
            m_hit.point = { info.worldPoint.x,
                            info.worldPoint.y,
                            info.worldPoint.z };
            m_hit.normal = { info.worldNormal.x,
                             info.worldNormal.y,
                             info.worldNormal.z };
            void* ud = info.body->getUserData();
            m_hit.object = ud ? static_cast<GameObject*>(ud) : nullptr;

            return info.hitFraction;  
        }
        RaycastHit    m_hit;
        bool          m_hasHit = false;
    };

    RaycastCallbackRp callback;

    m_world->raycast(rpRay, &callback);

    if (callback.m_hasHit) {
        callback.m_hit.distance = glm::distance(origin, callback.m_hit.point);
        outHit = callback.m_hit;
        return true;
    }
    return false;
}
