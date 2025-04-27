#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>

class reactphysics3d::PhysicsWorld;
class GameObject;
namespace Physics {

    struct RaycastHit {
        GameObject* object = nullptr;
        glm::vec3   point;
        glm::vec3   normal;
        float       distance = 0.0f;
    };
    class PhysicsEngine {
    public:
        static PhysicsEngine& Instance();
        reactphysics3d::PhysicsCommon& GetCommon();
        reactphysics3d::PhysicsWorld* GetWorld();

        void Update(float dt);

        bool Raycast(
            const glm::vec3& origin,
            const glm::vec3& direction,
            RaycastHit& outHit,
            float            maxDistance = 1000.0f);

    private:
        PhysicsEngine();
        ~PhysicsEngine();
        PhysicsEngine(const PhysicsEngine&) = delete;
        void operator=(const PhysicsEngine&) = delete;

        reactphysics3d::PhysicsCommon    m_common;
        reactphysics3d::PhysicsWorld* m_world = nullptr;
    };
}