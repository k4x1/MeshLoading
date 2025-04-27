#include "PhysicsEngine.h"
#include "GameObject.h"
using namespace Physics;PhysicsEngine& PhysicsEngine::Instance() {
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


bool PhysicsEngine::Raycast(
    const glm::vec3& origin,
    const glm::vec3& direction,
    RaycastHit& outHit,
    float            maxDistance)
{
    class RaycastCallbackRp : public reactphysics3d::RaycastCallback {
    public:
        RaycastCallbackRp(float maxDist) : m_maxDistance(maxDist) {}

        virtual reactphysics3d::decimal notifyRaycastHit(
            const reactphysics3d::RaycastInfo& info) override
        {
            m_hasHit = true;
            m_hit.point = { info.worldPoint.x,
                               info.worldPoint.y,
                               info.worldPoint.z };
            m_hit.normal = { info.worldNormal.x,
                               info.worldNormal.y,
                               info.worldNormal.z };
            m_hit.distance = static_cast<float>(info.hitFraction) * m_maxDistance;
            void* ud = info.body->getUserData();
            m_hit.object = ud ? static_cast<GameObject*>(ud) : nullptr;

            return info.hitFraction;
        }

        RaycastHit    m_hit;
        bool          m_hasHit = false;
    private:
        float         m_maxDistance;
    };

    glm::vec3 dirN = glm::normalize(direction);

    reactphysics3d::Vector3 rpOrigin(origin.x, origin.y, origin.z);
    reactphysics3d::Vector3 rpDir(dirN.x, dirN.y, dirN.z);

    reactphysics3d::Ray rpRay(rpOrigin, rpDir);
    RaycastCallbackRp callback(maxDistance);
    m_world->raycast(rpRay, &callback, true);

    if (callback.m_hasHit) {
        outHit = callback.m_hit;
        return true;
    }
    return false;
}