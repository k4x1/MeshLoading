#pragma once
#include "Component.h"
#include <reactphysics3d/reactphysics3d.h>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "ComponentFactory.h"
class BoxCollider : public ISerializableComponent {
public:
    bool   m_attached = false;
    glm::vec3 halfExtents{ 1,1,1 };
    reactphysics3d::BoxShape* shape = nullptr;

    void Start() override;
    void Update(float dt) override;
    void Render(class Camera* cam) override;
    void OnInspectorGUI() override;

    // ISerializableComponent
    nlohmann::json Serialize() const override {
        return {
            {"halfExtents", {halfExtents.x, halfExtents.y, halfExtents.z}}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        auto arr = j.value("halfExtents", std::vector<float>{1, 1, 1});
        halfExtents = { arr[0], arr[1], arr[2] };
    }
};

// register with the same macro
REGISTER_SERIALIZABLE_COMPONENT(BoxCollider);