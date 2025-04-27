#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <vector>        
//#include "Component.h"
//#include "ComponentFactory.h"
//#include "GameObject.h"
#include "EnginePluginAPI.h"

struct ENGINE_API Light {
    glm::vec3 color{ 1.0f,1.0f,1.0f };
    float     specularStrength{ 1.0f };
};

struct ENGINE_API PointLight : Light {
    glm::vec3 position{ 0.0f };
    float     attenuationConstant{ 1.0f };
    float     attenuationLinear{ 0.0f };
    float     attenuationExponent{ 0.0f };
};

struct ENGINE_API DirectionalLight : Light {
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
};

struct ENGINE_API SpotLight : Light {
    glm::vec3 position{ 0.0f };
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
    float     innerCutoff{ 0.0f };
    float     outerCutoff{ 0.0f };
    float     attenuationConstant{ 1.0f };
    float     attenuationLinear{ 0.0f };
    float     attenuationExponent{ 0.0f };
};

//class ENGINE_API DirectionalLightComponent : public Component {
//public:
//    Light     base;
//    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
//
//    void Start() override {}
//    void OnInspectorGUI() override {
//        ImGui::ColorEdit3("Color", glm::value_ptr(base.color));
//        ImGui::DragFloat("Specular", &base.specularStrength, 0.1f);
//        ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f);
//    }
//
//    static nlohmann::json Serialize(Component* c) {
//        auto* dl = dynamic_cast<DirectionalLightComponent*>(c);
//        if (!dl) return nullptr;
//        return {
//            {"color",     {dl->base.color.x, dl->base.color.y, dl->base.color.z}},
//            {"specular",  dl->base.specularStrength},
//            {"direction",{dl->direction.x, dl->direction.y, dl->direction.z}}
//        };
//    }
//}; 
//class ENGINE_API PointLightComponent : public Component, public PointLight {
//public:
//    void OnInspectorGUI() override {
//        ImGui::ColorEdit3("Color", glm::value_ptr(color));
//        ImGui::DragFloat("Specular", &specularStrength, 0.1f);
//        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
//        ImGui::DragFloat("Constant", &attenuationConstant, 0.01f);
//        ImGui::DragFloat("Linear", &attenuationLinear, 0.01f);
//        ImGui::DragFloat("Quadratic", &attenuationExponent, 0.001f);
//    }
//
//    static nlohmann::json Serialize(Component* c) {
//        auto* pl = dynamic_cast<PointLightComponent*>(c);
//        if (!pl) return nullptr;
//        return {
//            {"color",   {pl->color.x, pl->color.y, pl->color.z}},
//            {"specularStrength", pl->specularStrength},
//            {"position",{pl->position.x, pl->position.y, pl->position.z}},
//            {"attenuationConstant", pl->attenuationConstant},
//            {"attenuationLinear",   pl->attenuationLinear},
//            {"attenuationExponent", pl->attenuationExponent}
//        };
//    }
//};  
//class ENGINE_API SpotLightComponent : public Component, public SpotLight {
//public:
//    void OnInspectorGUI() override {
//        ImGui::ColorEdit3("Color", glm::value_ptr(color));
//        ImGui::DragFloat("Specular", &specularStrength, 0.1f);
//        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
//        ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f);
//        ImGui::DragFloat("InnerCutoff", &innerCutoff, 0.01f);
//        ImGui::DragFloat("OuterCutoff", &outerCutoff, 0.01f);
//        ImGui::DragFloat("Constant", &attenuationConstant, 0.01f);
//        ImGui::DragFloat("Linear", &attenuationLinear, 0.01f);
//        ImGui::DragFloat("Quadratic", &attenuationExponent, 0.001f);
//    }
//
//    static nlohmann::json Serialize(Component* c) {
//        auto* sl = dynamic_cast<SpotLightComponent*>(c);
//        if (!sl) return nullptr;
//        return {
//            {"color",               {sl->color.x, sl->color.y, sl->color.z}},
//            {"specularStrength",     sl->specularStrength},
//            {"position",            {sl->position.x, sl->position.y, sl->position.z}},
//            {"direction",           {sl->direction.x, sl->direction.y, sl->direction.z}},
//            {"innerCutoff",          sl->innerCutoff},
//            {"outerCutoff",          sl->outerCutoff},
//            {"attenuationConstant",  sl->attenuationConstant},
//            {"attenuationLinear",    sl->attenuationLinear},
//            {"attenuationExponent",  sl->attenuationExponent}
//        };
//    }
//}; 