#include "MeshModel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

MeshModel::MeshModel(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale, std::string ModelFilePath) :  m_position(Position), m_rotation(Rotation), m_scale(Scale)
{
    std::vector<VertexStandard> Vertices;
    tinyobj::ObjReaderConfig ReaderConfig;
    tinyobj::ObjReader Reader;

    if (!Reader.ParseFromFile(ModelFilePath, ReaderConfig)) {
        if (!Reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << Reader.Error();
        }
        exit(1);
    }

    if (!Reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << Reader.Warning();
    }

    auto& Attrib = Reader.GetAttrib();
    auto& Shapes = Reader.GetShapes();

    // Loop through the shapes of the object
    for (size_t ShapeIndex = 0; ShapeIndex < Shapes.size(); ShapeIndex++) {
        // Loop through the faces of the shape
        size_t ReadIndexOffset = 0;
        for (size_t FaceIndex = 0; FaceIndex < Shapes[ShapeIndex].mesh.num_face_vertices.size(); FaceIndex++) {
            size_t FaceVertexCount = size_t(Shapes[ShapeIndex].mesh.num_face_vertices[FaceIndex]);
            // Loop through the vertices of the face
            for (size_t VertexIndex = 0; VertexIndex < FaceVertexCount; VertexIndex++) {
                VertexStandard Vertex{};
                tinyobj::index_t TinyObjVertex = Shapes[ShapeIndex].mesh.indices[ReadIndexOffset + VertexIndex];
                Vertex.position = {
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 0],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 1],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 2]
                };
                if (TinyObjVertex.texcoord_index >= 0) { // Negative states no TexCoord data
                    Vertex.texcoord = {
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 0],
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 1]
                    };
                }
                /*if (TinyObjVertex.normal_index >= 0) { // Negative states no Normal data
                    Vertex.Normal = {
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 0],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 1],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 2]
                    };
                }*/
                Vertices.push_back(Vertex);
            }
            ReadIndexOffset += FaceVertexCount;
        }
    }
    m_drawType = GL_TRIANGLES;
    m_drawCount = (GLuint)Vertices.size();

    // Create the Vertex Array and associated buffers
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexStandard) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

    // Create the VertexAttribPointers for both Position and Texture coordinates
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    m_ModelMatrix = CalculateModelMatrix();

}

MeshModel::~MeshModel()
{
}

void MeshModel::Update(float DeltaTime)
{
}

void MeshModel::Render()
{
    glUniform1i(glGetUniformLocation(m_shader, "Texture0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ModelMat"), 1, GL_FALSE, &m_ModelMatrix[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(m_drawType, 0, m_drawCount);
    glBindVertexArray(0);
}
void MeshModel::LoadModel()
{
   
    
}

void MeshModel::InitTexture(const char* _filePath)
{
    int imageWidth = 0;
    int imageHeight = 0;
    int imageComponents = 0;
    stbi_set_flip_vertically_on_load(true);
    imageData = stbi_load(_filePath, &imageWidth, &imageHeight, &imageComponents, 0);
    if (imageData == nullptr) {
        std::cerr << "Failed to load image: " << std::endl;
        // Handle the error or return from the function.
    }
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    GLint LoadedComponents = (imageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, imageWidth, imageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_BLEND);

    // Set texture filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set texture wrapping mode based on the specified mode.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

void MeshModel::BindTexture()
{
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void MeshModel::SetShader(GLuint _shader)
{
    m_shader = _shader;
}

glm::mat4 MeshModel::CalculateModelMatrix()
{ // Calculate the translation, rotation, and scale matrices.
    glm::mat4 TranslationMat = glm::translate(glm::identity<glm::mat4>(), m_position);
    glm::mat4 RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
              RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
              RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 ScaleMat = glm::scale(glm::identity<glm::mat4>(), m_scale);
    // Combine the matrices to form the model matrix.
    return ScaleMat * TranslationMat * RotationMat;
}
