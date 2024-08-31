#include "NoiseScene.h"
#include "PerlinNoise.h"
#include <iostream>
#include "ShaderLoader.h"
#include "Skybox.h"
#include "Camera.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void NoiseScene::InitialSetup(GLFWwindow* _window, Camera* _camera) {
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    // Initialize camera
    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load skybox shader
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_noise = ShaderLoader::CreateProgram("Resources/Shaders/Noise.vert", "Resources/Shaders/Noise.frag");

    // Initialize skybox textures
    faces = {
           "Resources/Textures/skybox/Right.png",
           "Resources/Textures/skybox/Left.png",
           "Resources/Textures/skybox/Top.png",
           "Resources/Textures/skybox/Bottom.png",
           "Resources/Textures/skybox/Back.png",
           "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    // Generate Perlin noise and save as JPG and RAW
    GenerateAndSaveNoiseTexture();

    // Initialize the quad and shader program for rendering
    InitQuadAndShader();

    // Initialize the noise texture using the Texture class
    noiseTexture.InitTexture("Resources/Textures/NoiseTexture.jpg");
}

void NoiseScene::GenerateAndSaveNoiseTexture() {
    const int Width = 512;
    const int Height = 512;
    PerlinNoise noiseGenerator;
    uint8_t* Pixels = new uint8_t[Width * Height * 3];
    uint8_t* Heightmap = new uint8_t[Width * Height];

    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width; x++) {
            double noiseValue = noiseGenerator.TotalNoisePerPoint(x, y);
            noiseValue = (noiseValue + 1.0) * 0.5 * 255.0; // Scale to 0-255
            uint8_t color = static_cast<uint8_t>(noiseValue);

            // Apply gradient coloring (Fire effect: White, Yellow, Red, Black)
            glm::vec3 gradientColor = ApplyGradientColor(noiseValue / 255.0f);
            Pixels[(y * Width + x) * 3 + 0] = static_cast<uint8_t>(gradientColor.r * 255);
            Pixels[(y * Width + x) * 3 + 1] = static_cast<uint8_t>(gradientColor.g * 255);
            Pixels[(y * Width + x) * 3 + 2] = static_cast<uint8_t>(gradientColor.b * 255);

            // Save heightmap data
            Heightmap[y * Width + x] = color;
        }
    }

    // Save as JPG
    stbi_write_jpg("Resources/Textures/NoiseTexture.jpg", Width, Height, 3, Pixels, 100);

    // Save as RAW
    std::ofstream rawFile("Resources/Textures/NoiseHeightmap.raw", std::ios::binary);
    rawFile.write(reinterpret_cast<char*>(Heightmap), Width * Height);
    rawFile.close();

    delete[] Pixels;
    delete[] Heightmap;
}

glm::vec3 NoiseScene::ApplyGradientColor(float value) {
    
    if (value < 0.25f) {
        return glm::mix(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), value / 0.25f);
    }
    else if (value < 0.5f) {
        return glm::mix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), (value - 0.25f) / 0.25f);
    }
    else if (value < 0.75f) {
        return glm::mix(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), (value - 0.5f) / 0.25f);
    }
    else {
        return glm::mix(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), (value - 0.75f) / 0.25f);
    }
}

void NoiseScene::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

}


void NoiseScene::InitQuadAndShader() {
    // Define the vertices for a quad (two triangles)
    float quadVertices[] = {
        // positions        // texture coords
        -5.0f,  5.0f, 100.0f,  0.0f, 1.0f,
        -5.0f, -5.0f, 100.0f,  0.0f, 0.0f,
         5.0f, -5.0f, 100.0f,  1.0f, 0.0f,

        -5.0f,  5.0f, 100.0f,  0.0f, 1.0f,
         5.0f, -5.0f, 100.0f,  1.0f, 0.0f,
         5.0f,  5.0f, 100.0f,  1.0f, 1.0f
    };

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
}

void NoiseScene::Render() {
    // Clear the color buffer
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the skybox
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);

    // Render the noise texture on a quad
    RenderNoiseTexture();

    // Render animated noise on a second quad
 //   RenderAnimatedNoise();

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    // Swap buffers
    glfwSwapBuffers(Window);
}

void NoiseScene::RenderNoiseTexture() {
    glUseProgram(Program_noise);
    glBindVertexArray(quadVAO);

    // Create model matrix for the quad
    glm::mat4 model = glm::translate(glm::mat4(1.0f), quadPosition);

    // Pass the matrices to the shader
    glUniformMatrix4fv(glGetUniformLocation(Program_noise, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(Program_noise, "view"), 1, GL_FALSE, glm::value_ptr(camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_noise, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_noise, "texture1"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void NoiseScene::RenderAnimatedNoise() {
    // Generate new noise texture data
    const int Width = 512;
    const int Height = 512;
    PerlinNoise noiseGenerator;
    uint8_t* Pixels = new uint8_t[Width * Height * 3];

    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width; x++) {
            double noiseValue = noiseGenerator.TotalNoisePerPoint(x, y);
            noiseValue = (noiseValue + 1.0) * 0.5 * 255.0; // Scale to 0-255
            uint8_t color = static_cast<uint8_t>(noiseValue);

            glm::vec3 gradientColor = ApplyGradientColor(noiseValue / 255.0f);
            Pixels[(y * Width + x) * 3 + 0] = static_cast<uint8_t>(gradientColor.r * 255);
            Pixels[(y * Width + x) * 3 + 1] = static_cast<uint8_t>(gradientColor.g * 255);
            Pixels[(y * Width + x) * 3 + 2] = static_cast<uint8_t>(gradientColor.b * 255);
        }
    }

    // Update the texture with new noise data
    glBindTexture(GL_TEXTURE_2D, noiseTexture.GetId());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] Pixels;

    // Render the updated texture
    RenderNoiseTexture();
}
int NoiseScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        Update();
        Render();
        glfwPollEvents();
    }
    return 0;
}

NoiseScene::~NoiseScene() {
    delete skybox;
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}