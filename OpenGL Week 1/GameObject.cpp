
//having these separated breaks for some reason idfk


//#include "GameObject.h"
//#include "MeshRenderer.h"
//
//GameObject::GameObject(const std::string& name)
//    : name(name) {}
//
//
//void GameObject::AddChild(GameObject* child) {
//    child->parent = this;
//    children.push_back(child);
//}
//
//void GameObject::RemoveChild(GameObject* child) {
//    children.erase(
//        std::remove(children.begin(), children.end(), child),
//        children.end()
//    );
//    child->parent = nullptr;
//}
//
//glm::mat4 GameObject::GetWorldMatrix() const {
//    if (parent) {
//        return parent->GetWorldMatrix() * transform.GetLocalMatrix();
//    }
//    return transform.GetLocalMatrix();
//}
//
//template<typename T, typename... Args>
//T* GameObject::AddComponent(Args&&... args) {
//    T* comp = new T(std::forward<Args>(args)...);
//    comp->owner = this;
//    components.emplace_back(comp);
//    return comp;
//}
//
//template<typename T>
//T* GameObject::GetComponent() {
//    for (const auto& comp : components) {
//        if (T* t = dynamic_cast<T*>(comp.get())) {
//            return t;
//        }
//    }
//    return nullptr;
//}
//
//template<typename T>
//void GameObject::RemoveComponent() {
//    components.erase(
//        std::remove_if(components.begin(), components.end(),
//            [](const std::unique_ptr<Component>& comp) {
//                return dynamic_cast<T*>(comp.get()) != nullptr;
//            }),
//        components.end()
//    );
//}
//
//
//
//GameObject::~GameObject()
//{
//    for (GameObject* child : children) {
//        delete child;
//    }
//}
