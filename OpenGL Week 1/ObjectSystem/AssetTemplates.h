#pragma once
#include "../EnginePluginAPI.h"
#include <string>

class ENGINE_API AssetTemplates
{
public:
    static std::string GetMaterialTemplate()
    {
        return R"({
    "textureFilePath": "Resources/Textures/blankTex.png",
    "vertShaderPath": "Resources/Shaders/Texture.vert",
    "fragShaderPath": "Resources/Shaders/Texture.frag"
}
)";
    }

    static std::string GetScriptHeaderTemplate(const std::string& className)
    {
        std::string text = R"(#pragma once
#include "All.h"
#include <nlohmann/json.hpp>

class {{CLASS_NAME}} : public ISerializableComponent
{
public:
    {{CLASS_NAME}}() = default;
    ~{{CLASS_NAME}}() override = default;

    void Start() override;
    void Update(float dt) override;
    void OnInspectorGUI() override;

    nlohmann::json Serialize() const override;
    void Deserialize(const nlohmann::json& j) override;
};

REGISTER_SERIALIZABLE_COMPONENT({{CLASS_NAME}});
)";

        ReplaceAll(text, "{{CLASS_NAME}}", className);
        return text;
    }

    static std::string GetScriptSourceTemplate(const std::string& className)
    {
        std::string text = R"(#include "{{CLASS_NAME}}.h"
#include "ObjectSystem.GameObject.h"
#include "Utils/Debug.h"

void {{CLASS_NAME}}::Start()
{
}

void {{CLASS_NAME}}::Update(float dt)
{
}

void {{CLASS_NAME}}::OnInspectorGUI()
{
}

nlohmann::json {{CLASS_NAME}}::Serialize() const
{
    return nlohmann::json::object();
}

void {{CLASS_NAME}}::Deserialize(const nlohmann::json& j)
{
}
)";

        ReplaceAll(text, "{{CLASS_NAME}}", className);
        return text;
    }

private:
    static void ReplaceAll(std::string& text, const std::string& from, const std::string& to)
    {
        if (from.empty())
        {
            return;
        }

        size_t searchPosition = 0;

        while ((searchPosition = text.find(from, searchPosition)) != std::string::npos)
        {
            text.replace(searchPosition, from.length(), to);
            searchPosition += to.length();
        }
    }
};