#include "heightmapinfo.h"
#include "VertexStandard.h"
#include <fstream>
#include <iostream>

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
void BuildVertexData(const HeightMapInfo& BuildInfo, const std::vector<float>& HeightMap, std::vector<VertexStandard>& Vertices, float Height) {
    unsigned int VertexCount = BuildInfo.Width * BuildInfo.Depth;
    Vertices.resize(VertexCount);

    float HalfWidth = (BuildInfo.Width - 1) * BuildInfo.CellSpacing * 0.5f;
    float HalfDepth = (BuildInfo.Depth - 1) * BuildInfo.CellSpacing * 0.5f;
    float TexU = 1.0f / (BuildInfo.Width - 1);
    float TexV = 1.0f / (BuildInfo.Depth - 1);

    for (unsigned int Row = 0; Row < BuildInfo.Depth; Row++) {
        float PosZ = HalfDepth - (Row * BuildInfo.CellSpacing);
        for (unsigned int Col = 0; Col < BuildInfo.Width; Col++) {
            int Index = Row * BuildInfo.Width + Col;
            float PosX = -HalfWidth + (Col * BuildInfo.CellSpacing);
            float PosY = HeightMap[Index] * Height;

            Vertices[Index].position = glm::vec3(PosX, PosY, PosZ);
            Vertices[Index].texcoord = glm::vec2(Col * TexU, Row * TexV);
            Vertices[Index].normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}
void ComputeNormals(const HeightMapInfo& BuildInfo, const std::vector<float>& HeightMap, std::vector<VertexStandard>& Vertices) {
    float InvCellSpacing = 1.0f / (2.0f * BuildInfo.CellSpacing);

    for (unsigned int Row = 0; Row < BuildInfo.Depth; ++Row) {
        for (unsigned int Col = 0; Col < BuildInfo.Width; ++Col) {
            float RowNeg = HeightMap[(Row == 0 ? Row : Row - 1) * BuildInfo.Width + Col];
            float RowPos = HeightMap[(Row == BuildInfo.Depth - 1 ? Row : Row + 1) * BuildInfo.Width + Col];
            float ColNeg = HeightMap[Row * BuildInfo.Width + (Col == 0 ? Col : Col - 1)];
            float ColPos = HeightMap[Row * BuildInfo.Width + (Col == BuildInfo.Width - 1 ? Col : Col + 1)];

            float X = (RowNeg - RowPos);
            if (Row == 0 || Row == BuildInfo.Depth - 1) {
                X *= 2.0f;
            }

            float Y = (ColPos - ColNeg);
            if (Col == 0 || Col == BuildInfo.Width - 1) {
                Y *= 2.0f;
            }

            glm::vec3 TangentZ(0.0f, X * InvCellSpacing, 1.0f);
            glm::vec3 TangentX(1.0f, Y * InvCellSpacing, 0.0f);
            glm::vec3 Normal = glm::normalize(glm::cross(TangentZ, TangentX));

            Vertices[Row * BuildInfo.Width + Col].normal = Normal;
        }
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
float Average(const std::vector<float>& HeightMap, const HeightMapInfo& BuildInfo, unsigned int Row, unsigned int Col) {
    float sum = 0.0f;
    int count = 0;

    // Iterate over the 3x3 grid centered on (Row, Col)
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int newRow = Row + i;
            int newCol = Col + j;

            // Check if the neighboring cell is within bounds
            if (newRow >= 0 && newRow < BuildInfo.Depth && newCol >= 0 && newCol < BuildInfo.Width) {
                sum += HeightMap[newRow * BuildInfo.Width + newCol];
                count++;
            }
        }
    }

    // Return the average height
    return sum / count;
}
void SmoothHeights(std::vector<float>& HeightMap, const HeightMapInfo& BuildInfo) {
    std::vector<float> SmoothedMap(HeightMap.size());

    for (unsigned int Row = 0; Row < BuildInfo.Depth; Row++) {
        for (unsigned int Col = 0; Col < BuildInfo.Width; Col++) {
            SmoothedMap[Row * BuildInfo.Width + Col] = Average(HeightMap, BuildInfo, Row, Col);
        }
    }

    // Replace the original heightmap with the smoothed one
    HeightMap = SmoothedMap;
}
