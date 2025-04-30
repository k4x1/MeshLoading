#include "GameObject.h"
#include "IInspectable.h"
#include <gtc/type_ptr.hpp>
#include "ComponentFactory.h"
#include "Rigidbody.h" 
#include "Camera.h"
nlohmann::json TransformToJson(const Transform& t)
{
    return nlohmann::json{
        {"position", { t.position.x, t.position.y, t.position.z }},
        {"rotation", { t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z }},
        {"scale",    { t.scale.x,    t.scale.y,    t.scale.z    }}
    };
}

void TransformFromJson(const nlohmann::json& j, Transform& t)
{
    auto pos = j.at("position");
    t.position = glm::vec3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());

    auto rot = j.at("rotation");
    t.rotation = glm::quat(
        rot[0].get<float>(),
        rot[1].get<float>(),
        rot[2].get<float>(),
        rot[3].get<float>()
    );

    auto scale = j.at("scale");
    t.scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
}


GameObject::GameObject(const std::string& name)
    : name(name)
{
}

GameObject::~GameObject() {
    for (GameObject* child : children) {
        if (child != nullptr)
            delete child;
    }
}

void GameObject::AddChild(GameObject* child) {
    child->parent = this;
    children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child) {
    children.erase(
        std::remove(children.begin(), children.end(), child),
        children.end()
    );
    child->parent = nullptr;
}

glm::mat4 GameObject::GetWorldMatrix() const {
    return parent ? parent->GetWorldMatrix() * transform.GetLocalMatrix() : transform.GetLocalMatrix();
}

void GameObject::OnInspectorGUI() {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        if (ImGui::DragFloat3("Rotation", rotation, 0.5f)) {
            transform.rotation = glm::quat(glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2])));
        }
        ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);
    }

    for (auto& comp : components) {
        if (ImGui::CollapsingHeader(typeid(*comp).name())) {
            comp->OnInspectorGUI();
        }
    }
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");
    if (ImGui::BeginPopup("AddComponentPopup")) {
        for (auto& [typeName, entry] : ComponentFactory::Instance().GetRegistry()) {
            if (ImGui::MenuItem(typeName.c_str())) {
                Component* newComp =
                    ComponentFactory::Instance().Create(typeName, {}, this);
                if (newComp) {
                    newComp->owner = this;
                    components.emplace_back(newComp);
                }
            }
        }
        ImGui::EndPopup();
    }
}

void GameObject::Start() {
    for (auto& comp : components) {
        if (!comp)
        {
            DEBUG_LOG("null component");
            continue;
        }
        comp->Start();
    }
}
void GameObject::Update(float dt) {
    for (auto& comp : components) {
        if (!comp)
        {
            DEBUG_LOG("null component");
            continue;
        }

        comp->Update(dt);
      
    }
}

void GameObject::FixedUpdate(float fixedDt)
{
    for (auto& comp : components) {
        if (!comp)
        {
            DEBUG_LOG("null component");
            continue;
        }

        comp->FixedUpdate(fixedDt);

    }
}


void GameObject::Render(Camera* cam) {
    for (auto& comp : components) {
        comp->Render(cam);
    }
}
nlohmann::json SerializeGameObject(GameObject* obj) {
    nlohmann::json j;
    j["name"] = obj->name;
    j["transform"] = TransformToJson(obj->transform);

    j["components"] = nlohmann::json::array();
    for (auto& compPtr : obj->components) {
        Component* comp = compPtr.get();
        auto compJson = ComponentFactory::Instance().Serialize(comp);
        if (!compJson.is_null()) {
            j["components"].push_back(compJson);
        }
    }

    j["children"] = nlohmann::json::array();
    for (GameObject* child : obj->children) {
        j["children"].push_back(SerializeGameObject(child));
    }

    return j;
}
GameObject* DeserializeGameObject(const nlohmann::json& j)
{
    GameObject* obj = new GameObject(j.value("name", "Unnamed"));

    if (j.contains("transform"))
        TransformFromJson(j["transform"], obj->transform);

    if (j.contains("components") && j["components"].is_array())
    {
        for (auto& compJson : j["components"])
        {
            std::string type = compJson.value("type", "");
            Component* c = ComponentFactory::Instance()
                .Create(type, compJson, obj);
            if (!c) {
                std::cerr << "[Deserialize] Unknown component type: "
                    << type << "\n";
            }
        }
    }

    if (j.contains("children"))
        for (auto& childJson : j["children"])
            obj->AddChild(DeserializeGameObject(childJson));

    return obj;
}

