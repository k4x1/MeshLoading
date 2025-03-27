//#include "MeshComponent.h"
//#include <iostream>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <cstring>
//
//// Loads the mesh from file and sets up the VAO/VBO.
//MeshComponent::MeshComponent(const std::string& modelFilePath)
//    : VAO(0), drawCount(0), drawType(GL_TRIANGLES), shader(0), texture(0)
//{
//    std::vector<VertexStandard> vertices;
//    tinyobj::ObjReader reader;
//    tinyobj::ObjReaderConfig readerConfig;
//
//    if (!reader.ParseFromFile(modelFilePath, readerConfig)) {
//        if (!reader.Error().empty()) {
//            std::cerr << "TinyObjReader Error: " << reader.Error();
//        }
//        exit(1);
//    }
//    if (!reader.Warning().empty()) {
//        std::cout << "TinyObjReader Warning: " << reader.Warning();
//    }
//
//    auto& attrib = reader.GetAttrib();
//    auto& shapes = reader.GetShapes();
//
//    // Loop through shapes and faces to extract vertex data.
//    for (size_t s = 0; s < shapes.size(); s++) {
//        size_t indexOffset = 0;
//        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
//            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
//            for (size_t v = 0; v < fv; v++) {
//                VertexStandard vertex{};
//                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
//
//                // Vertex position.
//                vertex.position = {
//                    attrib.vertices[3 * size_t(idx.vertex_index) + 0],
//                    attrib.vertices[3 * size_t(idx.vertex_index) + 1],
//                    attrib.vertices[3 * size_t(idx.vertex_index) + 2]
//                };
//
//                // Texture coordinate.
//                if (idx.texcoord_index >= 0) {
//                    vertex.texcoord = {
//                        attrib.texcoords[2 * size_t(idx.texcoord_index) + 0],
//                        attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]
//                    };
//                }
//
//                // Normal.
//                if (idx.normal_index >= 0) {
//                    vertex.normal = {
//                        attrib.normals[3 * size_t(idx.normal_index) + 0],
//                        attrib.normals[3 * size_t(idx.normal_index) + 1],
//                        attrib.normals[3 * size_t(idx.normal_index) + 2]
//                    };
//                }
//
//                vertices.push_back(vertex);
//            }
//            indexOffset += fv;
//        }
//    }
//
//    drawCount = static_cast<GLuint>(vertices.size());
//
//    // Generate and bind VAO and VBO.
//    GLuint VBO;
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexStandard) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
//
//    // Setup attribute pointers (position, texcoord, normal).
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, normal));
//    glEnableVertexAttribArray(2);
//
//    glBindVertexArray(0);
//}
//
//// Destructor: clean up GPU resources.
//MeshComponent::~MeshComponent() {
//    glDeleteVertexArrays(1, &VAO);
//    // Also delete any buffers you created (like VBOs) as needed.
//}
//
//void MeshComponent::Update(float deltaTime) {
//    // Update any mesh-specific logic here if needed.
//}
//
//void MeshComponent::Render(const glm::mat4& modelMatrix) {
//    glUseProgram(shader);
//
//    // Pass the model matrix to the shader.
//    GLint modelLoc = glGetUniformLocation(shader, "ModelMat");
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
//
//    // Bind texture if available.
//    if (texture != 0) {
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glUniform1i(glGetUniformLocation(shader, "Texture0"), 0);
//    }
//
//    glBindVertexArray(VAO);
//    glDrawArrays(drawType, 0, drawCount);
//    glBindVertexArray(0);
//}
