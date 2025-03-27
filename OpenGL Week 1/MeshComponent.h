//#pragma once
//
//#include <string>
//#include <vector>
//#include <glew.h>
//#include <glm/glm.hpp>
//#include "VertexStandard.h"
//#include "tiny_obj_loader.h"
//
//class MeshComponent {
//public:
//    // Constructor: load the mesh from file.
//    MeshComponent(const std::string& modelFilePath);
//    ~MeshComponent();
//
//    // Render the mesh using the provided model matrix.
//    void Render(const glm::mat4& modelMatrix);
//
//    // Update method if you need to update any mesh-specific properties.
//    void Update(float deltaTime);
//
//    // Setters for texture and shader.
//    void SetTexture(GLuint textureID) { texture = textureID; }
//    void SetShader(GLuint shaderID) { shader = shaderID; }
//    GLuint GetShader() const { return shader; }
//
//    // (Optional) Add instancing or additional functionality here.
//private:
//    GLuint VAO;
//    GLuint drawCount;
//    int drawType; // e.g., GL_TRIANGLES
//
//    // Shader and texture IDs.
//    GLuint shader;
//    GLuint texture;
//
//    // (Optional) Instancing members and other mesh-related data.
//};
