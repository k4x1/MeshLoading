#pragma once
#include <iostream>
#include "SkyboxScene.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm.hpp>
#include <glew.h>
#include <glfw3.h>
#include "VertexStandard.h"
struct HeightMapInfo {
    std::string FilePath = "";
    unsigned int Width = 0;
    unsigned int Depth = 0;
    float CellSpacing = 1.0f;
};
bool LoadHeightMap(HeightMapInfo& BuildInfo, std::vector<float>& HeightMap) {
    unsigned int VertexCount = BuildInfo.Width * BuildInfo.Depth;
    std::vector<unsigned char> HeightValue(VertexCount);

    std::ifstream File;
    File.open(BuildInfo.FilePath, std::ios_base::binary);
    if (File) {
        File.read((char*)&HeightValue[0], (std::streamsize)HeightValue.size());
        File.close();
    }
    else {
        std::cout << "Error: Height map failed to load: " << BuildInfo.FilePath << std::endl;
        return false;
    }

    HeightMap.resize(VertexCount, 0);
    for (unsigned int i = 0; i < VertexCount; i++) {
        HeightMap[i] = (float)HeightValue[i];
    }
    return true;
}
void BuildVertexData(const HeightMapInfo& BuildInfo, const std::vector<float>& HeightMap, std::vector<VertexStandard>& Vertices) {
    unsigned int VertexCount = BuildInfo.Width * BuildInfo.Depth;
    Vertices.resize(VertexCount);

    float HalfWidth = (BuildInfo.Width - 1) * BuildInfo.CellSpacing * 0.5f;
    float HalfDepth = (BuildInfo.Depth - 1) * BuildInfo.CellSpacing * 0.5f;
    float TexU = 1.0f / (float)(BuildInfo.Width - 1);
    float TexV = 1.0f / (float)(BuildInfo.Depth - 1);

    for (unsigned int Row = 0; Row < BuildInfo.Width; Row++) {
        float PosZ = HalfDepth - (Row * BuildInfo.CellSpacing);
        for (unsigned int Col = 0; Col < BuildInfo.Depth; Col++) {
            int Index = Row * BuildInfo.Width + Col;
            float PosX = -HalfWidth + (Col * BuildInfo.CellSpacing);
            float PosY = HeightMap[Index];

            Vertices[Index].position = glm::vec3(PosX, PosY, PosZ);
            Vertices[Index].texcoord = glm::vec2(Col * TexU, Row * TexV);
            Vertices[Index].normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

void BuildIndexData(const HeightMapInfo& BuildInfo, std::vector<unsigned int>& Indices) {
    unsigned int IndexCount = (BuildInfo.Width - 1) * (BuildInfo.Depth - 1) * 6;
    Indices.resize(IndexCount);

    int k = 0;
    for (unsigned int Row = 0; Row < (BuildInfo.Depth - 1); Row++) {
        for (unsigned int Col = 0; Col < (BuildInfo.Width - 1); Col++) {
            Indices[k] = Row * BuildInfo.Width + Col;
            Indices[k + 1] = Row * BuildInfo.Width + Col + 1;
            Indices[k + 2] = (Row + 1) * BuildInfo.Width + Col;

            Indices[k + 3] = (Row + 1) * BuildInfo.Width + Col;
            Indices[k + 4] = Row * BuildInfo.Width + Col + 1;
            Indices[k + 5] = (Row + 1) * BuildInfo.Width + Col + 1;

            k += 6;
        }
    }
}

void ComputeNormals(std::vector<VertexStandard>& Vertices, const std::vector<unsigned int>& Indices) {
    unsigned int VertexCount = Vertices.size();
    unsigned int IndexCount = Indices.size();

    for (unsigned int i = 0; i < VertexCount; i++) {
        Vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    for (unsigned int i = 0; i < IndexCount; i += 3) {
        unsigned int Index0 = Indices[i];
        unsigned int Index1 = Indices[i + 1];
        unsigned int Index2 = Indices[i + 2];

        glm::vec3 v0 = Vertices[Index0].position;
        glm::vec3 v1 = Vertices[Index1].position;
        glm::vec3 v2 = Vertices[Index2].position;

        glm::vec3 Edge1 = v1 - v0;
        glm::vec3 Edge2 = v2 - v0;

        glm::vec3 FaceNormal = glm::normalize(glm::cross(Edge1, Edge2));

        Vertices[Index0].normal += FaceNormal;
        Vertices[Index1].normal += FaceNormal;
        Vertices[Index2].normal += FaceNormal;
    }

    for (unsigned int i = 0; i < VertexCount; i++) {
        Vertices[i].normal = glm::normalize(Vertices[i].normal);
    }
}

void BuildEBO(const std::vector<VertexStandard>& Vertices, const std::vector<unsigned int>& Indices, GLuint& VAO, GLuint& VBO, GLuint& EBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(VertexStandard), &Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
    // Vertex texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
    // Vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, normal));

    glBindVertexArray(0);
}