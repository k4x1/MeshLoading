#pragma once
#include <vector>
#include <reactphysics3d/collision/ContactPair.h>
#include "GameObject.h"
#include "EnginePluginAPI.h"
struct ENGINE_API CollisionInfo {
    GameObject* other;
    glm::vec3   contactPoint;      
    glm::vec3   contactNormal;     
};
class ENGINE_API ICollisionCallbacks {
public:
    virtual ~ICollisionCallbacks() = default;
    virtual void OnCollisionEnter(const std::vector<CollisionInfo>& contacts) {}
    virtual void OnCollisionStay(const std::vector<CollisionInfo>& contacts) {}
    virtual void OnCollisionExit(const std::vector<CollisionInfo>& contacts) {}
};