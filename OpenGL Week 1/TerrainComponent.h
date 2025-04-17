#pragma once

#include "Component.h"
#include "HeightMapInfo.h"
#include "Texture.h"
#include "ShaderLoader.h"
#include "Light.h"
#include <glew.h>    // For GLuint
#include <glm/glm.hpp>

class Camera;

class TerrainComponent : public Component {
public:
    // Constructor: takes heightmap info and optional height scale
    TerrainComponent(const HeightMapInfo& info, float heightScale = 1.0f);

    // Lifecycle
    void Start() override;
    void Render(Camera* cam) override;

private:
    HeightMapInfo m_info;
    float         m_heightScale;

    // OpenGL buffers
    GLuint        m_vao = 0;
    GLuint        m_vbo = 0;
    GLuint        m_ebo = 0;
    size_t        m_indexCount = 0;

    // Terrain textures
    Texture       grass;
    Texture       dirt;
    Texture       stone;
    Texture       snow;

    // Shader program
    GLuint        m_shader = 0;

    // (Optional) lighting parameters
    DirectionalLight m_dir1;
    DirectionalLight m_dir2;
    SpotLight        m_spot;
};
