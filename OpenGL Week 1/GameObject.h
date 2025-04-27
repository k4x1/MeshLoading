#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "IInspectable.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include "Component.h"
class Camera;

struct Transform {
    glm::vec3 position{ 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 GetLocalMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::toMat4(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }

    glm::vec3 GetForward() const {
        return rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 GetUp() const {
        return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    }
};

nlohmann::json TransformToJson(const Transform& t);
void TransformFromJson(const nlohmann::json& j, Transform& t);

class GameObject : public IInspectable {
public:
    std::string name;
    Transform transform;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
    std::vector<std::unique_ptr<Component>> components;

    GameObject(const std::string& name = "GameObject");
    virtual ~GameObject();

    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);

    glm::mat4 GetWorldMatrix() const;

    // Declaration only – definition moved to GameObject.cpp
    virtual void OnInspectorGUI() override;

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        T* comp = new T(std::forward<Args>(args)...);
        comp->owner = this;
        components.emplace_back(comp);
        return comp;
    }
    void AddComponentPointer(Component* c) {
        c->owner = this;
        components.emplace_back(c);
    }
    template<typename T>
    T* GetComponent() {
        for (const auto& comp : components) {
            if (T* t = dynamic_cast<T*>(comp.get()))
                return t;
        }
        return nullptr;
    }

   // template<typename T>
    void RemoveComponent(Component* comp) {
        components.erase(
            std::remove_if(components.begin(), components.end(),
                [comp](const std::unique_ptr<Component>& up) {
                    return up.get() == comp;
                }),
            components.end()
        );
    }

    void Update(float dt);
    void FixedUpdate(float fixedDt);
    void Start();
    void Render(Camera* cam);
};

nlohmann::json SerializeGameObject(GameObject* obj);
GameObject* DeserializeGameObject(const nlohmann::json& j);
