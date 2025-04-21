#pragma once
#include "MeshModel.h"
#include "Component.h"
#include <filesystem>
#include <memory>
#include <string>
#include "Texture.h"
namespace fs = std::filesystem;

class MeshRenderer : public Component {
public:
    MeshRenderer(const glm::vec3& pos,
        const glm::vec3& rot,
        const glm::vec3& scl,
        const std::string& modelPath);

    void Update(float dt) override;
    void Render(Camera* cam) override;
    void OnInspectorGUI() override;

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
