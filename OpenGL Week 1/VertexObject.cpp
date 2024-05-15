#include "VertexObject.h"
/*

// Returns the Vertex Array Object (VAO) ID associated with this VertexObject.
GLuint VertexObject::getVAO()
{
    return VAO_ID;
}

// Sets the Vertex Buffer Object (VBO) with the given vertices and size.
void VertexObject::setVBO(GLfloat* _vertices, GLsizei _VertSize)
{
    // Generate a new VBO and bind it.
    glGenBuffers(1, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
    // Fill the VBO with vertex data.
    glBufferData(GL_ARRAY_BUFFER, _VertSize, _vertices, GL_STATIC_DRAW);
}

// Sets the Vertex Array Object (VAO) with the given VAO ID or generates a new one if the ID is NULL.
void VertexObject::setVAO(GLuint _VAO)
{
    // If the provided VAO ID is NULL, generate a new VAO and set up its attributes.
    if (_VAO == NULL) {
        glGenVertexArrays(1, &VAO_ID);
        glBindVertexArray(VAO_ID);
        // Set up vertex attributes for position, normal, and texture coordinates.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }
    // If a VAO ID is provided, simply bind it.
    else {
        VAO_ID = _VAO;
        glBindVertexArray(VAO_ID);
    }
}

// Sets the Element Buffer Object (EBO) with the given indices and size.
void VertexObject::setEBO(GLuint* _IndicesList, GLsizei _indicesSize)
{
    // Generate a new EBO and bind it.
    glGenBuffers(1, &EB_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB_ID);
    // Fill the EBO with index data.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesSize, _IndicesList, GL_STATIC_DRAW);
    // Unbind the VAO to prevent accidental modifications.
    glBindVertexArray(0);
}

// Draws the VertexObject using the specified number of indices.
void VertexObject::draw(GLuint _indicesCount)
{
    // Bind the VAO and draw the object using the indices.
    glBindVertexArray(VAO_ID);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, nullptr);
    // Unbind the VAO and the shader program.
    glBindVertexArray(0);
    glUseProgram(0);
}

// Initializes the texture for the VertexObject using the specified file path.
void VertexObject::InitTexture(const char* _filePath)
{
    // Load the image data from the file.
    stbi_set_flip_vertically_on_load(true);
    imageData = stbi_load(_filePath, &imageWidth, &imageHeight, &imageComponents, 0);
    if (imageData == nullptr) {
        std::cerr << "Failed to load image: " << _filePath << std::endl;
        // Handle the error or return from the function.
    }
    // Generate a new texture and bind it.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Determine the number of color components in the image.
    GLint LoadedComponents = (imageComponents == 4) ? GL_RGBA : GL_RGB;
    // Load the image data into the texture.
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, imageWidth, imageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, imageData);
    // Generate mipmaps for the texture.
    glGenerateMipmap(GL_TEXTURE_2D);
    // Free the image data and unbind the texture.
    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Enable blending for the texture.
    glEnable(GL_BLEND);
}

// Sets the texture mode for the VertexObject.
void VertexObject::setTexture(Modes _mode)
{
    // Activate the first texture unit.
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set texture filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set texture wrapping mode based on the specified mode.
    switch (_mode) {
    case MIRRORED:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;
    case ANIMATED:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    default:
        std::cout << "invalid mode";
        break;
    }
    // Set the uniform for the texture in the shader program.
    glUniform1i(glGetUniformLocation(Shader_ID, "Texture0"), 0);
}

// Sets the shader program for the VertexObject.
void VertexObject::setShader(GLuint _Shader)
{
    Shader_ID = _Shader;
    glUseProgram(Shader_ID);
}

// Defines the model matrix for the VertexObject based on the given position, rotation, and scale.
void VertexObject::defineModelMatrix(glm::vec3 QuadPosition, float QuadRotation, glm::vec3 QuadScale)
{
    // Calculate the translation, rotation, and scale matrices.
    TranslationMat = glm::translate(glm::identity<glm::mat4>(), QuadPosition);
    RotationMat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(QuadRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    ScaleMat = glm::scale(glm::identity<glm::mat4>(), QuadScale);
    // Combine the matrices to form the model matrix.
    ModelMat = ScaleMat * TranslationMat * RotationMat;
}
    */