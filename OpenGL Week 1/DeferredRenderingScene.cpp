#include "DeferredRenderingScene.h"
#include "HeightMapInfo.h"
#include <iostream>

void DeferredRenderingScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    Window = _window;
    camera = _camera;
    m_GeometryBuffer = new GeometryBuffer();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 0.0f));

    // Load shaders
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");
    Program_color = ShaderLoader::CreateProgram("Resources/Shaders/Color.vert", "Resources/Shaders/Color.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");
    Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
    Program_GeometryPass = ShaderLoader::CreateProgram("Resources/Shaders/GeometryPass.vert", "Resources/Shaders/GeometryPass.frag");
    Program_ScreenQuadLightingPass = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/LightingPass.frag");
 
    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    InitializeModels();
    SetupLights();
    //SetupTerrain();
    SetupQuad();

}

void DeferredRenderingScene::Update()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Update other scene elements
    model->Update(deltaTime);
}
void DeferredRenderingScene::Render() {
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
    // Bind the geometry buffer
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    m_GeometryBuffer->Bind();
    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the geometry pass shader
    glUseProgram(Program_GeometryPass);

    // Render all models
    for (auto m : models) {
        m->PassUniforms(camera);
        m->BindTexture();
        m->Render(Program_GeometryPass);
    }

 
    
    // Unbind the geometry buffer
    m_GeometryBuffer->Unbind();
    
    
    // Render screen quad
    glUseProgram(Program_ScreenQuadLightingPass);
    m_GeometryBuffer->PopulateProgram(Program_ScreenQuadLightingPass);
    glUniform1i(glGetUniformLocation(Program_ScreenQuadLightingPass, "PointLightCount"), pointLights.size());
    glUniform3fv(glGetUniformLocation(Program_ScreenQuadLightingPass, "CameraPos"), 1,&camera->m_position[0]);
    glUniform1f(glGetUniformLocation(Program_ScreenQuadLightingPass, "AmbientStrength"), 0.2f);
    glUniform3fv(glGetUniformLocation(Program_ScreenQuadLightingPass, "AmbientColor"), 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
    // Iterate over each light and pass its properties
    for (size_t i = 0; i < pointLights.size(); ++i) {
        std::string baseName = "PointLightArray[" + std::to_string(i) + "]";
        
        glUniform3fv(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".position").c_str()), 1,&pointLights[i].position[0]);
        glUniform3fv(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".color").c_str()), 1, &pointLights[i].color[0]);
        glUniform1f(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".specularStrength").c_str()), pointLights[i].specularStrength);
        glUniform1f(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".attenuationConstant").c_str()), pointLights[i].attenuationConstant);
        glUniform1f(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".attenuationLinear").c_str()), pointLights[i].attenuationLinear);
        glUniform1f(glGetUniformLocation(Program_ScreenQuadLightingPass, (baseName + ".attenuationExponent").c_str()), pointLights[i].attenuationExponent);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Write depth
    m_GeometryBuffer->WriteDepth();
    
    // Render lights
    glUseProgram(Program_color);
    for (int i = 0; i< lightSourceModels.size(); i++)
    {
        lightSourceModels[i]->BindTexture();
        lightSourceModels[i]->PassUniforms(camera);
        lightSourceModels[i]->PassColorUniforms(pointLights[i].color.r, pointLights[i].color.g, pointLights[i].color.b, 1.0f);
        lightSourceModels[i]->Render(lightSourceModels[i]->GetShader());
    }

    // Skybox
    skybox->Render(view, projection);


    glUseProgram(0);
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}


void DeferredRenderingScene::InitializeModels() {
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);


    int rows = 10; 
    int cols = 10; 
    float spacing = 200.0f; 

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            glm::vec3 position = glm::vec3(col * spacing, 0.0f, row * spacing);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(0.05f);
            MeshModel* newModel = new MeshModel(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
            newModel->SetTexture(ancientTex.GetId());
            newModel->SetShader(Program_GeometryPass);
            models.push_back(newModel);
        }
    }
    faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

}
void DeferredRenderingScene::SetupLights() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // Initialize point lights
    for (int i = 0; i < 10; ++i) {
        PointLight light;
        light.position = glm::vec3(rand() % 100 , 10.0f, rand() % 100);
        light.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
        light.specularStrength = 0.5f;
        light.attenuationConstant = 1.0f;
        light.attenuationLinear = 0.022f;
        light.attenuationExponent = 0.0019f;
        pointLights.push_back(light);

        // Create light source model
        MeshModel* lightSource = new MeshModel(light.position, glm::vec3(0), glm::vec3(1), "Resources/Models/Sphere.obj");
        lightSource->SetTexture(blankTex.GetId());
        lightSource->SetShader(Program_color);
        lightSourceModels.push_back(lightSource);
    }

    glDisable(GL_CULL_FACE);
}
void DeferredRenderingScene::SetupTerrain() {
    HeightMapInfo heightMapInfo = { "Resources/Heightmaps/heightmap.raw", 512, 512, 1.0f };
    std::vector<float> heightMap;
    if (LoadHeightMap(heightMapInfo, heightMap)) {
        SmoothHeights(heightMap, heightMapInfo);

        std::vector<VertexStandard> vertices;
        std::vector<unsigned int> indices;
        BuildVertexData(heightMapInfo, heightMap, vertices, 0.2f);
        BuildIndexData(heightMapInfo, indices);
        ComputeNormals(heightMapInfo, heightMap, vertices);

        GLuint VAO, VBO, EBO;
        BuildEBO(vertices, indices, VAO, VBO, EBO);
        terrainVAO = VAO;
        terrainVBO = VBO;
        terrainEBO = EBO;
        this->indices = indices;

        terrainTexture.InitTexture("Resources/Textures/fakerRomance.png");
        grassTexture.InitTexture("Resources/Textures/grass.png");
        dirtTexture.InitTexture("Resources/Textures/dirt.png");
        stoneTexture.InitTexture("Resources/Textures/rocks.png");
        snowTexture.InitTexture("Resources/Textures/snow.png");
    }
}
void DeferredRenderingScene::SetupQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
void DeferredRenderingScene::RenderPostProcessing() {
    // Set the viewport to the window size
    glViewport(0, 0, 800, 800);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the post-processing shader
    glUseProgram(postProcessingShader);

    // Bind the framebuffer texture to be used as the input for post-processing
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), static_cast<int>(currentEffect));

    // Bind the vertex array for the quad
    glBindVertexArray(quadVAO);

    // Disable depth testing for post-processing
    glDisable(GL_DEPTH_TEST);

    // Bind the texture from the framebuffer
    m_FrameBuffer->BindTexture();

    // Draw the quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}
int DeferredRenderingScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        Update();
        Render();
        glfwPollEvents();
    }
    return 0;
}

DeferredRenderingScene::~DeferredRenderingScene() {
    delete model;
    delete skybox;
    delete instanceModel;

    if (m_FrameBuffer) {
        delete m_FrameBuffer;
        m_FrameBuffer = nullptr;
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}


