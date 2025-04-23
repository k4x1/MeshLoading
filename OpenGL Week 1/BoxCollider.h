#pragma once
#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

class BoxCollider : public Component {
public:
    glm::vec3 halfExtents{ 1.0f,1.0f,1.0f };

    void Start() override;
    void OnInspectorGUI() override;
    void Render(class Camera* cam) override;
    reactphysics3d::BoxShape* shape = nullptr;
    static nlohmann::json Serialize(Component* comp);
};
