#pragma once

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>      
#include "Component.h"     
#include "ComponentFactory.h"
#include "GameObject.h"   
#include "EnginePluginAPI.h"

class ENGINE_API Camera : public ISerializableComponent {
public:
    float m_FOV = 90.0f;
    float m_width = 800.0f;
    float m_height = 800.0f;

    glm::mat4 m_view = glm::mat4(0);
    glm::mat4  m_projection = glm::mat4(0);

    Camera() = default;
    virtual ~Camera() = default;

    void InitCamera(float width, float height);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    void Matrix(float nearPlane,
        float farPlane,
        GLuint shaderID = 0,
        const char* uniform = nullptr);

    nlohmann::json Serialize() const override {
        return {
            {"FOV",    m_FOV},
            {"width",  m_width},
            {"height", m_height}
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        m_FOV = j.value("FOV", m_FOV);
        m_width = j.value("width", m_width);
        m_height = j.value("height", m_height);
    }
};

REGISTER_SERIALIZABLE_COMPONENT(Camera);
