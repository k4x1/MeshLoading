#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

// Forward declaration of Component
class Component;

// Simple Transform struct for clarity
struct Transform {
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f }; // Euler angles (degrees)
    glm::vec3 scale{ 1.0f };

    glm::mat4 GetLocalMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::quat q = glm::quat(glm::radians(rotation));
        glm::mat4 R = glm::mat4_cast(q);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }
};

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

class Component {
public:
    GameObject* owner = nullptr;
    virtual ~Component() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(class Camera* cam) {}
};
