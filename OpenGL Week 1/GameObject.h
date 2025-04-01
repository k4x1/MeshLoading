#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Utils.h"
#include <nlohmann/json.hpp>
class Camera;

class Component;

struct Transform {
public:
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

inline nlohmann::json TransformToJson(const Transform& t)
{
    return nlohmann::json{
        {"position", { t.position.x, t.position.y, t.position.z }},
        {"rotation", { t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z }},
        {"scale",    { t.scale.x,    t.scale.y,    t.scale.z    }}
    };
}

inline void TransformFromJson(const nlohmann::json& j, Transform& t)
{
    auto pos = j.at("position");
    t.position = glm::vec3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());

    auto rot = j.at("rotation");
    // Expecting the quaternion in order: [w, x, y, z]
    t.rotation = glm::quat(
        rot[0].get<float>(),
        rot[1].get<float>(),
        rot[2].get<float>(),
        rot[3].get<float>()
    );

    auto scale = j.at("scale");
    t.scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
}
class GameObject {
public:
    std::string name;
    Transform transform;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
    std::vector<std::unique_ptr<Component>> components;

    GameObject(const std::string& name = "GameObject") : name(name) {}
    ~GameObject() {
        for (GameObject* child : children) {
            delete child;
        }
    }

    void AddChild(GameObject* child) {
        child->parent = this;
        children.push_back(child);
    }

    void RemoveChild(GameObject* child) {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end()
        );
        child->parent = nullptr;
    }

    glm::mat4 GetWorldMatrix() const {
        return parent ? parent->GetWorldMatrix() * transform.GetLocalMatrix() : transform.GetLocalMatrix();
    }

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        T* comp = new T(std::forward<Args>(args)...);
        comp->owner = this;
        components.emplace_back(comp);
        return comp;
    }

    template<typename T>
    T* GetComponent() {
        for (const auto& comp : components) {
            if (T* t = dynamic_cast<T*>(comp.get()))
                return t;
        }
        return nullptr;
    }

    template<typename T>
    void RemoveComponent() {
        components.erase(
            std::remove_if(components.begin(), components.end(),
                [](const std::unique_ptr<Component>& comp) {
                    return dynamic_cast<T*>(comp.get()) != nullptr;
                }),
            components.end()
        );
    }

    void Update(float dt) {
        for (auto& comp : components) {
            comp->Update(dt);
        }
    }

    void Start() {
        for (auto& comp : components) {
            comp->Start();
        }
    }

    void Render(Camera* cam) {
        for (auto& comp : components) {
            comp->Render(cam);
        }
    }
};

inline nlohmann::json SerializeGameObject(GameObject* obj)
{
    nlohmann::json j;
    j["name"] = obj->name;
    j["transform"] = TransformToJson(obj->transform);

    j["children"] = nlohmann::json::array();
    for (GameObject* child : obj->children)
    {
        j["children"].push_back(SerializeGameObject(child));
    }
    return j;
}

inline GameObject* DeserializeGameObject(const nlohmann::json& j)
{
    GameObject* obj = new GameObject(j.value("name", "Unnamed"));

    if (j.contains("transform"))
    {
        TransformFromJson(j["transform"], obj->transform);
    }

    if (j.contains("children"))
    {
        for (const auto& childJson : j["children"])
        {
            GameObject* child = DeserializeGameObject(childJson);
            obj->AddChild(child);
        }
    }
    return obj;
}
