#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include "Utils.h"
#include <nlohmann/json.hpp>

// Forward declaration of Component
class Component;



class GameObject {
public:
    std::string name;
    Transform transform;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
    std::vector<std::unique_ptr<Component>> components;

    // Inline constructor and destructor.
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
};
inline json SerializeGameObject(GameObject* obj)
{
    json j;
    j["name"] = obj->name;
    j["transform"] = TransformToJson(obj->transform);


    j["children"] = json::array();
    for (GameObject* child : obj->children)
    {
        j["children"].push_back(SerializeGameObject(child));
    }
    return j;
}

inline GameObject* DeserializeGameObject(const json& j)
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
class Component {
public:
    GameObject* owner = nullptr;
    virtual ~Component() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(class Camera* cam) {}
};
