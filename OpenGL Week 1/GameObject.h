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

    // Computes the local transformation matrix.
    glm::mat4 GetLocalMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        // Convert Euler angles to radians then create a quaternion.
        glm::quat q = glm::quat(glm::radians(rotation));
        glm::mat4 R = glm::mat4_cast(q);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }
};

/// GameObject class represents an entity in the scene.
class GameObject {
public:
    std::string name;
    Transform transform;

    // Pointer to parent (if any) and list of child GameObjects.
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;

    // List of components attached to this GameObject.
    std::vector<std::unique_ptr<Component>> components;

    GameObject(const std::string& name = "GameObject");

    ~GameObject();

    // Adds a child GameObject.
    void AddChild(GameObject* child);

    // Removes a child GameObject.
    void RemoveChild(GameObject* child);

    // Recursively computes the world transformation matrix.
    glm::mat4 GetWorldMatrix() const;

    // Adds a component of type T to this GameObject.
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    // Retrieves the first component of type T attached to this GameObject.
    template<typename T>
    T* GetComponent();
    // Removes all components of type T from this GameObject.
    template<typename T>
    void RemoveComponent();
};


class Component {
public:
    GameObject* owner = nullptr;
    virtual ~Component() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(class Camera* cam) {}
};

