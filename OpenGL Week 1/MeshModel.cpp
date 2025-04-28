
#include "MeshModel.h"
#include "Camera.h"
#include "Debug.h"      
#include "tiny_obj_loader_impl.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

MeshModel::MeshModel(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, std::string _modelFilePath)
    : m_position(_position), m_rotation(_rotation), m_scale(_scale)
{
    std::vector<VertexStandard> Vertices;
    tinyobj::ObjReaderConfig ReaderConfig;
    tinyobj::ObjReader Reader;

    if (!Reader.ParseFromFile(_modelFilePath, ReaderConfig)) {
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

    for (size_t ShapeIndex = 0; ShapeIndex < Shapes.size(); ShapeIndex++) {
        size_t ReadIndexOffset = 0;
        for (size_t FaceIndex = 0; FaceIndex < Shapes[ShapeIndex].mesh.num_face_vertices.size(); FaceIndex++) {
            size_t FaceVertexCount = size_t(Shapes[ShapeIndex].mesh.num_face_vertices[FaceIndex]);
            for (size_t VertexIndex = 0; VertexIndex < FaceVertexCount; VertexIndex++) {
                VertexStandard Vertex{};
                tinyobj::index_t TinyObjVertex = Shapes[ShapeIndex].mesh.indices[ReadIndexOffset + VertexIndex];

                Vertex.position = {
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 0],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 1],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 2]
                };

                if (TinyObjVertex.texcoord_index >= 0) {
                    Vertex.texcoord = {
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 0],
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 1]
                    };
                }
                if (TinyObjVertex.normal_index >= 0) {
                    Vertex.normal = {
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 0],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 1],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 2]
                    };
                }
                Vertices.push_back(Vertex);
            }
            ReadIndexOffset += FaceVertexCount;
        }
    }

    m_drawType = GL_TRIANGLES;
    m_drawCount = static_cast<GLuint>(Vertices.size());

    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexStandard) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, normal));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    m_modelMatrix = CalculateModelMatrix();
}

MeshModel::~MeshModel()
{
}

void MeshModel::Update(float DeltaTime)
{
}

void MeshModel::Render(GLuint shader)
{
    glUseProgram(shader);

    if (m_texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        GLint loc = glGetUniformLocation(shader, "Texture0");
        if (loc >= 0) glUniform1i(loc, 0);
    }

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "ModelMat"),
        1, GL_FALSE,
        glm::value_ptr(m_modelMatrix)
    );

    glBindVertexArray(VAO);
    glDrawArrays(m_drawType, 0, m_drawCount);
    glBindVertexArray(0);

    glUseProgram(0);
}

void MeshModel::BindTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    if (m_texture != NULL) {
        glUniform1i(glGetUniformLocation(m_shader, "Texture0"), 0);
    }

}

void MeshModel::SetTexture(GLuint _textureID)
{
    m_texture = _textureID;
}

void MeshModel::SetShader(GLuint _shader)
{
    m_shader = _shader;
}

GLuint MeshModel::GetShader()
{
    return m_shader;
}

void MeshModel::SetPosition(glm::vec3 _newPos)
{
    m_position = _newPos;
    m_modelMatrix = CalculateModelMatrix();
}

void MeshModel::SetRotation(glm::vec3 _newRot)
{
    m_rotation = _newRot;
    m_modelMatrix = CalculateModelMatrix();
}


void MeshModel::PassPointUniforms(Camera* _camRef, PointLight* _lightArr, unsigned int _lightCount)
{

    
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
  
    _camRef->Matrix(0.01f, 1000.0f, m_shader, "VPMatrix");
  //  glUniform3fv(glGetUniformLocation(m_shader, "CameraPos"), 1, &_camRef->m_position[0]);
}
void MeshModel::PassSpotLightUniforms(SpotLight _spotLight)
{
    std::string uniformName = "SptLight.position";
    glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_spotLight.position));
    uniformName = "SptLight.direction";
    glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_spotLight.direction));
    uniformName = "SptLight.color";
    glUniform3fv(glGetUniformLocation(m_shader, uniformName.c_str()), 1, glm::value_ptr(_spotLight.color));
    uniformName = "SptLight.specularStrength";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.specularStrength);
    uniformName = "SptLight.attenuationConstant";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.attenuationConstant);
    uniformName = "SptLight.attenuationLinear";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.attenuationLinear);
    uniformName = "SptLight.attenuationExponent";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.attenuationExponent);
    uniformName = "SptLight.innerCutoff";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.innerCutoff);
    uniformName = "SptLight.outerCutoff";
    glUniform1f(glGetUniformLocation(m_shader, uniformName.c_str()), _spotLight.outerCutoff);
}

glm::vec3 MeshModel::GetPosition()
{
    return m_position;
}

glm::mat4 MeshModel::CalculateModelMatrix()
{
    glm::mat4 TranslationMat = glm::translate(glm::identity<glm::mat4>(), m_position);
    glm::mat4 RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    RotationMat = glm::rotate(RotationMat, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 ScaleMat = glm::scale(glm::identity<glm::mat4>(), m_scale);
    return ScaleMat * TranslationMat * RotationMat;
}
