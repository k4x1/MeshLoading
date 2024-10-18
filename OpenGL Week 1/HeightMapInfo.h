#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <glew.h>

struct HeightMapInfo {
    std::string FilePath = "";
    unsigned int Width = 0;
    unsigned int Depth = 0;
    float CellSpacing = 1.0f;
};

struct VertexStandard;

bool LoadHeightMap(HeightMapInfo& BuildInfo, std::vector<float>& HeightMap);
void BuildIndexData(const HeightMapInfo& BuildInfo, std::vector<unsigned int>& Indices);
void BuildVertexData(const HeightMapInfo& BuildInfo, const std::vector<float>& HeightMap, std::vector<VertexStandard>& Vertices, float Height = 1.0f);
void ComputeNormals(const HeightMapInfo& BuildInfo, const std::vector<float>& HeightMap, std::vector<VertexStandard>& Vertices);
void BuildEBO(const std::vector<VertexStandard>& Vertices, const std::vector<unsigned int>& Indices, GLuint& VAO, GLuint& VBO, GLuint& EBO);
float Average(const std::vector<float>& HeightMap, const HeightMapInfo& BuildInfo, unsigned int Row, unsigned int Col);
void SmoothHeights(std::vector<float>& HeightMap, const HeightMapInfo& BuildInfo);
