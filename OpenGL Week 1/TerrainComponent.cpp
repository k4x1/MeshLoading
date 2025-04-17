#include "TerrainComponent.h"
#include "ShaderLoader.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"
#include "VertexStandard.h"
// constructor
TerrainComponent::TerrainComponent(const HeightMapInfo& info, float heightScale)
    : m_info(info), m_heightScale(heightScale)
{}

void TerrainComponent::Start() {
    // 1) load & smooth
    std::vector<float> heights;
    if (!LoadHeightMap(m_info, heights))
        return;
    SmoothHeights(heights, m_info);

    // 2) build vertices & indices
    std::vector<VertexStandard> vertices;
    std::vector<unsigned int> indices;
    BuildVertexData(m_info, heights, vertices, m_heightScale);
    BuildIndexData(m_info, indices);
    ComputeNormals(m_info, heights, vertices);

    m_indexCount = indices.size();

    // 3) upload to GPU
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexStandard),
        vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard),
        (void*)offsetof(VertexStandard, position));
    glEnableVertexAttribArray(0);
    // texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard),
        (void*)offsetof(VertexStandard, texcoord));
    glEnableVertexAttribArray(1);
    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard),
        (void*)offsetof(VertexStandard, normal));
    glEnableVertexAttribArray(2);

    //  unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 4) load textures
    grass.InitTexture("Resources/Textures/grass.png");
    dirt.InitTexture("Resources/Textures/dirt.png");
    stone.InitTexture("Resources/Textures/rocks.png");
    snow.InitTexture("Resources/Textures/snow.png");

    // 5) grab terrain shader
    m_shader = ShaderLoader::CreateProgram(
        "Resources/Shaders/Terrain.vert",
        "Resources/Shaders/Terrain.frag");
}

void TerrainComponent::Render(Camera* cam) {
    if (!m_vao || !m_shader) return;

    glUseProgram(m_shader);

    // set camera uniforms
    cam->Matrix(0.01f, 1000.0f, m_shader, "viewProj");

    // set light uniforms here, or fetch from scene...
    // glUniform3fv(... DirLight.direction ...);

    // bind textures into units 0–3
    auto bindTex = [&](GLuint unit, const Texture& t, const char* name) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, t.GetId());
        glUniform1i(glGetUniformLocation(m_shader, name), unit);
        };
    bindTex(0, grass, "grassTexture");
    bindTex(1, dirt, "dirtTexture");
    bindTex(2, stone, "stoneTexture");
    bindTex(3, snow, "snowTexture");

    // draw
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // unbind textures
    for (int i = 0; i < 4; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
}
