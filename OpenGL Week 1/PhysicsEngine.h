#pragma once
#include <reactphysics3d/reactphysics3d.h>

#include <vector>
#include <glm.hpp>
#include "EnginePluginAPI.h"
#include "ICollisionCallbacks.h"

class GameObject;
namespace Physics {
    struct ENGINE_API RaycastHit {
        GameObject* object = nullptr;
        glm::vec3   point = glm::vec3(0);
        glm::vec3   normal = glm::vec3(0);
        float       distance = 0.0f;
    };
    class ENGINE_API PhysicsEngine {
    public:
        static PhysicsEngine& Instance();
        reactphysics3d::PhysicsCommon& GetCommon();
        reactphysics3d::PhysicsWorld* GetWorld();

        void FixedUpdate(float fixedDt);

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
        class CollisionListener : public reactphysics3d::EventListener {
        public:
            void onContact(const reactphysics3d::CollisionCallback::CallbackData& cd) override {
                for (rp3d::uint32 i = 0; i < cd.getNbContactPairs(); ++i) {
                    auto pair = cd.getContactPair(i);

                    std::vector<CollisionInfo> infos;
                    GameObject* go1 = static_cast<GameObject*>(pair.getBody1()->getUserData());
                    GameObject* go2 = static_cast<GameObject*>(pair.getBody2()->getUserData());

                    for (rp3d::uint32 j = 0; j < pair.getNbContactPoints(); ++j) {
                        auto pt = pair.getContactPoint(j);
                        infos.push_back(
                        {
                            nullptr,
                            { 
                                pt.getLocalPointOnCollider1().x,
                                pt.getLocalPointOnCollider1().y,
                                pt.getLocalPointOnCollider1().z 
                            },
                            { 
                                pt.getWorldNormal().x,
                                pt.getWorldNormal().y,
                                pt.getWorldNormal().z 
                            }
                        });
                    }

                    auto dispatchTo = [&](GameObject* me, GameObject* other) {
                        if (!me) return;
                        for (auto& c : infos) c.other = other;
                        for (auto& compPtr : me->components) {
                            if (auto* cb = dynamic_cast<ICollisionCallbacks*>(compPtr.get())) {
                                switch (pair.getEventType()) {
                                case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart:
                                    cb->OnCollisionEnter(infos); break;
                                case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay:
                                    cb->OnCollisionStay(infos);  break;
                                case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit:
                                    cb->OnCollisionExit(infos);  break;
                                }
                            }
                        }
                        };

                    dispatchTo(go1, go2);
                    dispatchTo(go2, go1);
                }
            }
        } m_collisionListener;
    };
 
}