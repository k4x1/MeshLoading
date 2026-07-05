#include "Material.h"

#include "ShaderLoader.h"
#include "../Utils/Debug.h"
#include "../Editor/Inspector/InspectorPropertyList.h"

#include <fstream>
#include <filesystem>
#include <gtc/type_ptr.hpp>

static std::string ShaderPropertyTypeToString(ShaderPropertyType type)
{
    switch (type)
    {
    case ShaderPropertyType::Float:
        return "Float";

    case ShaderPropertyType::Int:
        return "Int";

    case ShaderPropertyType::Bool:
        return "Bool";

    case ShaderPropertyType::Vec2:
        return "Vec2";

    case ShaderPropertyType::Vec3:
        return "Vec3";

    case ShaderPropertyType::Vec4:
        return "Vec4";

    case ShaderPropertyType::Color:
        return "Color";

    case ShaderPropertyType::Texture2D:
        return "Texture2D";

    default:
        return "Unknown";
    }
}

static ShaderPropertyType ShaderPropertyTypeFromString(const std::string& value)
{
    if (value == "Float")
    {
        return ShaderPropertyType::Float;
    }

    if (value == "Int")
    {
        return ShaderPropertyType::Int;
    }

    if (value == "Bool")
    {
        return ShaderPropertyType::Bool;
    }

    if (value == "Vec2")
    {
        return ShaderPropertyType::Vec2;
    }

    if (value == "Vec3")
    {
        return ShaderPropertyType::Vec3;
    }

    if (value == "Vec4")
    {
        return ShaderPropertyType::Vec4;
    }

    if (value == "Color")
    {
        return ShaderPropertyType::Color;
    }

    if (value == "Texture2D")
    {
        return ShaderPropertyType::Texture2D;
    }

    return ShaderPropertyType::Unknown;
}

bool Material::LoadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (file.is_open() == false)
    {
        DEBUG_ERR("Failed to open material: " << filePath);
        return false;
    }

    nlohmann::json json;

    try
    {
        file >> json;
    }
    catch (const std::exception& exception)
    {
        DEBUG_ERR("Failed to parse material: " << filePath << " " << exception.what());
        return false;
    }

    Deserialize(json);

    path = filePath;

    if (name.empty())
    {
        name = std::filesystem::path(filePath).stem().string();
    }

    return true;
}

bool Material::SaveToFile(const std::string& filePath) const
{
    std::ofstream file(filePath);

    if (file.is_open() == false)
    {
        DEBUG_ERR("Failed to save material: " << filePath);
        return false;
    }

    file << Serialize().dump(4);
    return true;
}
void Material::SyncPropertiesWithShader(GLuint shaderProgram)
{
    const std::vector<ShaderPropertyInfo>& shaderProperties =
        ShaderLoader::GetMaterialProperties(shaderProgram);

    for (const ShaderPropertyInfo& shaderProperty : shaderProperties)
    {
        std::unordered_map<std::string, MaterialPropertyValue>::iterator existingProperty =
            properties.find(shaderProperty.name);

        if (existingProperty != properties.end())
        {
            existingProperty->second.type = shaderProperty.type;
            continue;
        }

        MaterialPropertyValue newValue =
            CreateDefaultPropertyValueFromShader(shaderProgram, shaderProperty);

        properties[shaderProperty.name] = newValue;
        isDirty = true;

    }
}

void Material::ApplyPropertiesToShader(GLuint shaderProgram) const
{
    for (const std::pair<const std::string, MaterialPropertyValue>& propertyPair : properties)
    {
        const std::string& propertyName = propertyPair.first;
        const MaterialPropertyValue& value = propertyPair.second;

        GLint location = glGetUniformLocation(shaderProgram, propertyName.c_str());

        if (location < 0)
        {
            continue;
        }

        if (value.type == ShaderPropertyType::Float)
        {
            glUniform1f(location, value.floatValue);
        }
        else if (value.type == ShaderPropertyType::Int)
        {
            glUniform1i(location, value.intValue);
        }
        else if (value.type == ShaderPropertyType::Bool)
        {
            glUniform1i(location, value.boolValue ? 1 : 0);
        }
        else if (value.type == ShaderPropertyType::Vec2)
        {
            glUniform2fv(location, 1, glm::value_ptr(value.vec2Value));
        }
        else if (value.type == ShaderPropertyType::Vec3)
        {
            glUniform3fv(location, 1, glm::value_ptr(value.vec3Value));
        }
        else if (value.type == ShaderPropertyType::Vec4 ||
                 value.type == ShaderPropertyType::Color)
        {
            glUniform4fv(location, 1, glm::value_ptr(value.vec4Value));
        }
    }
}


nlohmann::json Material::Serialize() const
{
    nlohmann::json json;

    json["name"] = name;
    json["vertShaderPath"] = vertShaderPath;
    json["fragShaderPath"] = fragShaderPath;

    nlohmann::json propertiesJson = nlohmann::json::object();

    for (const std::pair<const std::string, MaterialPropertyValue>& propertyPair : properties)
    {
        const std::string& propertyName = propertyPair.first;
        const MaterialPropertyValue& value = propertyPair.second;

        nlohmann::json propertyJson;
        propertyJson["type"] = ShaderPropertyTypeToString(value.type);

        if (value.type == ShaderPropertyType::Float)
        {
            propertyJson["value"] = value.floatValue;
        }
        else if (value.type == ShaderPropertyType::Int)
        {
            propertyJson["value"] = value.intValue;
        }
        else if (value.type == ShaderPropertyType::Bool)
        {
            propertyJson["value"] = value.boolValue;
        }
        else if (value.type == ShaderPropertyType::Vec2)
        {
            propertyJson["value"] = { value.vec2Value.x, value.vec2Value.y };
        }
        else if (value.type == ShaderPropertyType::Vec3)
        {
            propertyJson["value"] = { value.vec3Value.x, value.vec3Value.y, value.vec3Value.z };
        }
        else if (value.type == ShaderPropertyType::Vec4 ||
                 value.type == ShaderPropertyType::Color)
        {
            propertyJson["value"] = {
                value.vec4Value.x,
                value.vec4Value.y,
                value.vec4Value.z,
                value.vec4Value.w
            };
        }
        else if (value.type == ShaderPropertyType::Texture2D)
        {
            propertyJson["value"] = value.texturePath;
        }

        propertiesJson[propertyName] = propertyJson;
    }

    json["properties"] = propertiesJson;

    return json;
}
void Material::Deserialize(const nlohmann::json& json)
{
    name = json.value("name", name);
    vertShaderPath = json.value("vertShaderPath", vertShaderPath);
    fragShaderPath = json.value("fragShaderPath", fragShaderPath);

    properties.clear();

    if (json.contains("properties") == false)
    {
        return;
    }

    const nlohmann::json& propertiesJson = json["properties"];

    for (nlohmann::json::const_iterator iterator = propertiesJson.begin();
         iterator != propertiesJson.end();
         ++iterator)
    {
        std::string propertyName = iterator.key();
        const nlohmann::json& propertyJson = iterator.value();

        MaterialPropertyValue value;
        value.type = ShaderPropertyTypeFromString(propertyJson.value("type", "Unknown"));

        if (propertyJson.contains("value"))
        {
            const nlohmann::json& storedValue = propertyJson["value"];

            if (value.type == ShaderPropertyType::Float)
            {
                value.floatValue = storedValue.get<float>();
            }
            else if (value.type == ShaderPropertyType::Int)
            {
                value.intValue = storedValue.get<int>();
            }
            else if (value.type == ShaderPropertyType::Bool)
            {
                value.boolValue = storedValue.get<bool>();
            }
            else if (value.type == ShaderPropertyType::Vec2 && storedValue.is_array() && storedValue.size() >= 2)
            {
                value.vec2Value = glm::vec2(storedValue[0].get<float>(), storedValue[1].get<float>());
            }
            else if (value.type == ShaderPropertyType::Vec3 && storedValue.is_array() && storedValue.size() >= 3)
            {
                value.vec3Value = glm::vec3(
                    storedValue[0].get<float>(),
                    storedValue[1].get<float>(),
                    storedValue[2].get<float>()
                );
            }
            else if ((value.type == ShaderPropertyType::Vec4 || value.type == ShaderPropertyType::Color) &&
                     storedValue.is_array() &&
                     storedValue.size() >= 4)
            {
                value.vec4Value = glm::vec4(
                    storedValue[0].get<float>(),
                    storedValue[1].get<float>(),
                    storedValue[2].get<float>(),
                    storedValue[3].get<float>()
                );
            }
            else if (value.type == ShaderPropertyType::Texture2D)
            {
                value.texturePath = storedValue.get<std::string>();
            }
        }

        properties[propertyName] = value;
    }
}
void Material::BuildInspectorProperties(
    InspectorPropertyList& propertyList,
    GLuint shaderProgram
)
{
    propertyList.AddString(
        "Name",
        &name,
        [this]()
        {
            isDirty = true;
        }
    );

    propertyList.AddAssetPath(
        "Vertex Shader",
        &vertShaderPath,
        { AssetType::Script },
        [this]()
        {
            isDirty = true;
        }
    );

    propertyList.AddAssetPath(
        "Fragment Shader",
        &fragShaderPath,
        { AssetType::Script },
        [this]()
        {
            isDirty = true;
        }
    );

    if (shaderProgram == 0)
    {
        return;
    }

    SyncPropertiesWithShader(shaderProgram);

    const std::vector<ShaderPropertyInfo>& shaderProperties =
        ShaderLoader::GetMaterialProperties(shaderProgram);

    for (const ShaderPropertyInfo& shaderProperty : shaderProperties)
    {
        MaterialPropertyValue& value = properties[shaderProperty.name];

        if (shaderProperty.type == ShaderPropertyType::Float)
        {
            propertyList.AddFloat(
                shaderProperty.name,
                &value.floatValue,
                0.01f,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Int)
        {
            propertyList.AddInt(
                shaderProperty.name,
                &value.intValue,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Bool)
        {
            propertyList.AddBool(
                shaderProperty.name,
                &value.boolValue,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Vec2)
        {
            propertyList.AddVec2(
                shaderProperty.name,
                &value.vec2Value,
                0.01f,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Vec3)
        {
            propertyList.AddVec3(
                shaderProperty.name,
                &value.vec3Value,
                0.01f,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Vec4)
        {
            propertyList.AddVec4(
                shaderProperty.name,
                &value.vec4Value,
                0.01f,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Color)
        {
            propertyList.AddColor(
                shaderProperty.name,
                &value.vec4Value,
                [this]()
                {
                    isDirty = true;
                }
            );
        }
        else if (shaderProperty.type == ShaderPropertyType::Texture2D)
        {
            propertyList.AddAssetPath(
                shaderProperty.name,
                &value.texturePath,
                { AssetType::Texture },
                [this]()
                {
                    isDirty = true;
                }
            );
        }
    }
}

MaterialPropertyValue Material::CreateDefaultPropertyValueFromShader(GLuint shaderProgram,
    const ShaderPropertyInfo& shaderProperty) const
{
    MaterialPropertyValue value;
    value.type = shaderProperty.type;

    GLint location = glGetUniformLocation(shaderProgram, shaderProperty.name.c_str());

    if (location < 0)
    {
        if (shaderProperty.type == ShaderPropertyType::Texture2D)
        {
            value.texturePath = "Resources/Textures/blankTex.png";
        }

        return value;
    }

    if (shaderProperty.type == ShaderPropertyType::Float)
    {
        GLfloat shaderValue = 0.0f;
        glGetUniformfv(shaderProgram, location, &shaderValue);
        value.floatValue = shaderValue;
    }
    else if (shaderProperty.type == ShaderPropertyType::Int)
    {
        GLint shaderValue = 0;
        glGetUniformiv(shaderProgram, location, &shaderValue);
        value.intValue = shaderValue;
    }
    else if (shaderProperty.type == ShaderPropertyType::Bool)
    {
        GLint shaderValue = 0;
        glGetUniformiv(shaderProgram, location, &shaderValue);
        value.boolValue = shaderValue != 0;
    }
    else if (shaderProperty.type == ShaderPropertyType::Vec2)
    {
        GLfloat shaderValue[2] = { 0.0f, 0.0f };
        glGetUniformfv(shaderProgram, location, shaderValue);

        value.vec2Value = glm::vec2(
            shaderValue[0],
            shaderValue[1]
        );
    }
    else if (shaderProperty.type == ShaderPropertyType::Vec3)
    {
        GLfloat shaderValue[3] = { 0.0f, 0.0f, 0.0f };
        glGetUniformfv(shaderProgram, location, shaderValue);

        value.vec3Value = glm::vec3(
            shaderValue[0],
            shaderValue[1],
            shaderValue[2]
        );
    }
    else if (shaderProperty.type == ShaderPropertyType::Vec4)
    {
        GLfloat shaderValue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glGetUniformfv(shaderProgram, location, shaderValue);

        value.vec4Value = glm::vec4(
            shaderValue[0],
            shaderValue[1],
            shaderValue[2],
            shaderValue[3]
        );
    }
    else if (shaderProperty.type == ShaderPropertyType::Color)
    {
        GLfloat shaderValue[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glGetUniformfv(shaderProgram, location, shaderValue);

        value.vec4Value = glm::vec4(
            shaderValue[0],
            shaderValue[1],
            shaderValue[2],
            shaderValue[3]
        );
    }
    else if (shaderProperty.type == ShaderPropertyType::Texture2D)
    {
        value.texturePath = "Resources/Textures/blankTex.png";
    }

    return value;
}
