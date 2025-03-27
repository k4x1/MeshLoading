#pragma once
#include "GameObject.h" 
#include <nlohmann/json.hpp>


using json = nlohmann::json;
struct Transform {
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 GetLocalMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::quat q = glm::quat(glm::radians(rotation));
        glm::mat4 R = glm::mat4_cast(q);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }
};
inline json TransformToJson(const Transform& t)
{
    return json{
        {"position", { t.position.x, t.position.y, t.position.z }},
        {"rotation", { t.rotation.x, t.rotation.y, t.rotation.z }},
        {"scale",    { t.scale.x,    t.scale.y,    t.scale.z    }}
    };
}

inline void TransformFromJson(const json& j, Transform& t)
{
    auto pos = j.at("position");
    t.position = glm::vec3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());

    auto rot = j.at("rotation");
    t.rotation = glm::vec3(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>());

    auto scale = j.at("scale");
    t.scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
}

