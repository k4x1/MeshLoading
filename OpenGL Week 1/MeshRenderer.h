#pragma once
#include "MeshModel.h"
#include "Component.h"
#include <filesystem>
#include <memory>
#include <string>
#include "Texture.h"
#include <glm/glm.hpp>  
namespace fs = std::filesystem;

class MeshRenderer : public ISerializableComponent {
public:
  //  MeshRenderer() = default;
    MeshRenderer(
        const glm::vec3& pos = glm::vec3(0),
        const glm::vec3& rot = glm::vec3(0),
        const glm::vec3& scl = glm::vec3(1),
        const std::string& modelPath = ""
    );

    void Update(float dt) override;
    void Render(Camera* cam) override;
    void OnInspectorGUI() override;
    nlohmann::json Serialize() const override {
        return {
            { "modelFilePath",   modelFilePath   },
            { "textureFilePath", textureFilePath },
            { "vertShaderPath",  vertShaderPath  },
            { "fragShaderPath",  fragShaderPath  }
        };
    }
    void Deserialize(const nlohmann::json& j) override {
        modelFilePath = j.value("modelFilePath", modelFilePath);
        textureFilePath = j.value("textureFilePath", textureFilePath);
        vertShaderPath = j.value("vertShaderPath", vertShaderPath);
        fragShaderPath = j.value("fragShaderPath", fragShaderPath);
        Reload();
    }

    // public so factory/inspector can touch them:
    std::string modelFilePath;
    std::string textureFilePath;
    std::string vertShaderPath;
    std::string fragShaderPath;
    Texture texture;
    void Reload();  // rebuilds mesh/texture/shader

private:
    std::unique_ptr<MeshModel> mesh;
    GLuint shaderProgram = 0;
    GLuint textureID = 0;

    // timestamps for change‑detection
    fs::file_time_type _modelTime;
    fs::file_time_type _texTime;
    fs::file_time_type _vertTime;
    fs::file_time_type _fragTime;

    void _checkFileUpdates();
};
