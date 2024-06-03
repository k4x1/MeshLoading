/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : MeshModel.cpp
Description : Implementation file for the MeshModel class, which manages the loading, updating, and rendering of a 3D mesh model.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/

#include "MeshModel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Constructor for MeshModel
MeshModel::MeshModel(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, std::string _modelFilePath)
    : m_position(_position), m_rotation(_rotation), m_scale(_scale)
{
    std::vector<VertexStandard> Vertices;
    tinyobj::ObjReaderConfig ReaderConfig;
    tinyobj::ObjReader Reader;

    // Parse the OBJ file
    if (!Reader.ParseFromFile(_modelFilePath, ReaderConfig)) {
        if (!Reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << Reader.Error();
        }
        exit(1);
    }

    // Print warnings if any
    if (!Reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << Reader.Warning();
    }

    auto& Attrib = Reader.GetAttrib();
    auto& Shapes = Reader.GetShapes();

    // Loop through the shapes of the object
    for (size_t ShapeIndex = 0; ShapeIndex < Shapes.size(); ShapeIndex++) {
        size_t ReadIndexOffset = 0;
        // Loop through the faces of the shape
        for (size_t FaceIndex = 0; FaceIndex < Shapes[ShapeIndex].mesh.num_face_vertices.size(); FaceIndex++) {
            size_t FaceVertexCount = size_t(Shapes[ShapeIndex].mesh.num_face_vertices[FaceIndex]);
            // Loop through the vertices of the face
            for (size_t VertexIndex = 0; VertexIndex < FaceVertexCount; VertexIndex++) {
                VertexStandard Vertex{};
                tinyobj::index_t TinyObjVertex = Shapes[ShapeIndex].mesh.indices[ReadIndexOffset + VertexIndex];

                // Set vertex position
                Vertex.position = {
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 0],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 1],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 2]
                };

                // Set vertex texture coordinates if available
                if (TinyObjVertex.texcoord_index >= 0) {
                    Vertex.texcoord = {
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 0],
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 1]
                    };
                }
                if (TinyObjVertex.normal_index >= 0) { // Negative states no Normal data
                    Vertex.normal = {
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 0],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 1],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 2]
                    };
                }
                // Add vertex to the list
                Vertices.push_back(Vertex);
            }
            ReadIndexOffset += FaceVertexCount;
        }
    }

    m_drawType = GL_TRIANGLES;
    m_drawCount = static_cast<GLuint>(Vertices.size());

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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, normal));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Calculate the initial model matrix
    m_modelMatrix = CalculateModelMatrix();
}

// Destructor for MeshModel
MeshModel::~MeshModel()
{
}

// Update function (currently empty)
void MeshModel::Update(float DeltaTime)
{
}

// Render function
void MeshModel::Render()
{
    if(m_texture!= NULL){
        glUniform1i(glGetUniformLocation(m_shader, "Texture0"), 0);
    }
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ModelMat"), 1, GL_FALSE, &m_modelMatrix[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(m_drawType, 0, m_drawCount);
    glBindVertexArray(0);
}

// Bind the texture
void MeshModel::BindTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

// Set the texture ID
void MeshModel::SetTexture(GLuint _textureID)
{
    m_texture = _textureID;
}

// Set the shader program
void MeshModel::SetShader(GLuint _shader)
{
    m_shader = _shader;
}

// Set the position and update the model matrix
void MeshModel::SetPosition(glm::vec3 _newPos)
{
    m_position = _newPos;
    m_modelMatrix = CalculateModelMatrix();
}

void MeshModel::PassPointUniforms(Camera* _camRef, PointLight* _lightArr, unsigned int _lightCount)
{

    
    glUniform3fv(glGetUniformLocation(m_shader, "CameraPos"), 1, &_camRef->m_position[0]);
    for (unsigned int i = 0; i < _lightCount; ++i)
    {
        std::string uniformName = "PointLightArray[" + std::to_string(i) + "].position";
        glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_lightArr[i].position));

        uniformName = "PointLightArray[" + std::to_string(i) + "].color";
        glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_lightArr[i].color));

        uniformName = "PointLightArray[" + std::to_string(i) + "].specularStrength";
        glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _lightArr[i].specularStrength);

        uniformName = "PointLightArray[" + std::to_string(i) + "].attenuationConstant";
        glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _lightArr[i].attenuationConstant);
        
        uniformName = "PointLightArray[" + std::to_string(i) + "].attenuationLinear";
        glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _lightArr[i].attenuationLinear);

        uniformName = "PointLightArray[" + std::to_string(i) + "].attenuationExponent";
        glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _lightArr[i].attenuationExponent);
    }

    glUniform1ui(glGetUniformLocation(m_shader, "PointLightCount"), _lightCount);
}

void MeshModel::PassColorUniforms(float _r, float _g, float _b, float _a)
{
    GLint colorLocation = glGetUniformLocation(m_shader, "Color");
    glUniform4f(colorLocation, _r, _g, _b, _a);
}

void MeshModel::PassDirectionalUniforms(DirectionalLight _light)
{
    std::string uniformName = "DirLight.direction";
    glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_light.direction));
    uniformName = "DirLight.color";
    glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_light.color));
    uniformName = "DirLight.specularStrength";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _light.specularStrength);
}

void MeshModel::PassUniforms(Camera* _camRef)
{
    glUseProgram(m_shader);
    _camRef->Matrix(0.01f, 1000.0f, m_shader, "VPMatrix");
}

// Get the current position
glm::vec3 MeshModel::GetPosition()
{
    return m_position;
}

// Calculate the model matrix based on position, rotation, and scale
glm::mat4 MeshModel::CalculateModelMatrix()
{
    glm::mat4 TranslationMat = glm::translate(glm::identity<glm::mat4>(), m_position);
    glm::mat4 RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 ScaleMat = glm::scale(glm::identity<glm::mat4>(), m_scale);
    return ScaleMat * TranslationMat * RotationMat;
}
