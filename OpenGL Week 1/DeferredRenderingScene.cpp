#include "DeferredRenderingScene.h"
#include "HeightMapInfo.h"
#include <iostream>

void DeferredRenderingScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);

    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 0.0f));

    // Load shaders
    geometryShader = ShaderLoader::CreateProgram("Resources/Shaders/GeometryPass.vert", "Resources/Shaders/GeometryPass.frag");
    lightingShader = ShaderLoader::CreateProgram("Resources/Shaders/LightingPass.vert", "Resources/Shaders/LightingPass.frag");
    lightSourceShader = ShaderLoader::CreateProgram("Resources/Shaders/LightSource.vert", "Resources/Shaders/LightSource.frag");
    Program_terrain = ShaderLoader::CreateProgram("Resources/Shaders/Terrain.vert", "Resources/Shaders/Terrain.frag");
     Program_skybox = ShaderLoader::CreateProgram("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    

    for (int i = 0; i < 20; ++i) {
        glm::vec3 position = glm::vec3(rand() % 300 - 150, 0.0f, rand() % 300 - 150);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(0.05f);
        MeshModel* newModel = new MeshModel(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
        newModel->SetTexture(ancientTex.GetId());
        newModel->SetShader(geometryShader);
        models.push_back(newModel);
    }

    // Initialize point lights
    for (int i = 0; i < 10; ++i) {
        PointLight light;
        light.position = glm::vec3(rand() % 300 - 150, 10.0f, rand() % 300 - 150);
        light.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
        light.specularStrength = 2.0f;
        light.attenuationConstant = 1.0f;
        light.attenuationLinear = 0.045f;
        light.attenuationExponent = 0.0075f;
        pointLights.push_back(light);

        // Create light source model
        MeshModel* lightSource = new MeshModel(light.position, glm::vec3(0), glm::vec3(1), "Resources/Models/Sphere.obj");
        lightSource->SetTexture(blankTex.GetId());
        lightSource->SetShader(lightSourceShader);
        lightSourceModels.push_back(lightSource);
    }

    SetupGBuffer();

    // Set up quad for post-processing
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
    instanceModel = new InstanceMeshModel(glm::vec3(0), glm::vec3(0), glm::vec3(1.0f), "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    instanceModel->SetTexture(ancientTex.GetId());
    instanceModel->SetShader(geometryShader);

    for (int i = 0; i < 20; ++i) {
        glm::vec3 randomPosition = glm::vec3(rand() % 300 - 150, 0.0f, rand() % 300 - 150);
        glm::vec3 randomRotation = glm::vec3((rand() % 21) - 10);
        glm::vec3 randomScale = glm::vec3(((rand() % 10 + 5) / 200.0f));
        instanceModel->AddInstance(randomPosition, randomRotation, randomScale);
    }
    instanceModel->initBuffer();


    for (int i = 0; i < 20; ++i) {
        glm::vec3 randomPosition = glm::vec3(rand() % 300 - 150, 0.0f, rand() % 300 - 150);
        glm::vec3 randomRotation = glm::vec3((rand() % 21) - 10);
        glm::vec3 randomScale = glm::vec3(((rand() % 10 + 5) / 200.0f));
        instanceModel->AddInstance(randomPosition, randomRotation, randomScale);
    }
    instanceModel->initBuffer();

    // Initialize point lights as instanced models
    InstanceMeshModel* lightInstanceModel = new InstanceMeshModel(glm::vec3(0), glm::vec3(0), glm::vec3(1.0f), "Resources/Models/Sphere.obj");
    lightInstanceModel->SetTexture(blankTex.GetId());
    lightInstanceModel->SetShader(lightSourceShader);

    for (int i = 0; i < 10; ++i) {
        glm::vec3 lightPosition = glm::vec3(rand() % 300 - 150, 10.0f, rand() % 300 - 150);
        glm::vec3 lightRotation = glm::vec3(0.0f);
        glm::vec3 lightScale = glm::vec3(1.0f);
        lightInstanceModel->AddInstance(lightPosition, lightRotation, lightScale);
    }
    lightInstanceModel->initBuffer();

    // Initialize terrain
    HeightMapInfo heightMapInfo = { "Resources/Heightmaps/heightmap.raw", 512, 512, 1.0f };
    std::vector<float> heightMap;
    if (LoadHeightMap(heightMapInfo, heightMap)) {
        SmoothHeights(heightMap, heightMapInfo);  // Apply smoothing

        std::vector<VertexStandard> vertices;
        std::vector<unsigned int> indices;
        BuildVertexData(heightMapInfo, heightMap, vertices, 0.2f); // 0.2 is height
        BuildIndexData(heightMapInfo, indices);
        ComputeNormals(heightMapInfo, heightMap, vertices);

        GLuint VAO, VBO, EBO;
        BuildEBO(vertices, indices, VAO, VBO, EBO);
        terrainVAO = VAO;
        terrainVBO = VBO;
        terrainEBO = EBO;
        this->indices = indices; // Store indices for rendering

        // Load terrain texture
        terrainTexture.InitTexture("Resources/Textures/fakerRomance.png");

        grassTexture.InitTexture("Resources/Textures/grass.png");
        dirtTexture.InitTexture("Resources/Textures/dirt.png");
        stoneTexture.InitTexture("Resources/Textures/rocks.png");
        snowTexture.InitTexture("Resources/Textures/snow.png");
    }

}

void DeferredRenderingScene::Update()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Update other scene elements
    for (auto& model : models) {
        model->Update(deltaTime);
    }

    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;

    static bool tabPressed = false;
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed)
    {
        tabPressed = true;
        currentEffect = static_cast<PostProcessEffect>((static_cast<int>(currentEffect) + 1) % 3);
    }
    if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        tabPressed = false;
    }
}


void DeferredRenderingScene::SetupGBuffer()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 800, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 800, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Albedo + Specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Depth buffer
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 800);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderingScene::GeometryPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(geometryShader);
    instanceModel->Render(geometryShader);
    // Render the terrain
    glUseProgram(Program_terrain);
    glBindVertexArray(terrainVAO);



    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "view"), 1, GL_FALSE, glm::value_ptr(camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "grassTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dirtTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "dirtTexture"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, stoneTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "stoneTexture"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snowTexture.GetId());
    glUniform1i(glGetUniformLocation(Program_terrain, "snowTexture"), 3);

    // Draw the terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DeferredRenderingScene::LightingPass() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(lightingShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

    // Set camera position uniform
    glUniform3fv(glGetUniformLocation(lightingShader, "CameraPos"), 1, glm::value_ptr(camera->m_position));

    // Set ambient light uniform
    glUniform1f(glGetUniformLocation(lightingShader, "AmbientStrength"), AmbientStrength);
    glUniform3fv(glGetUniformLocation(lightingShader, "AmbientColor"), 1, glm::value_ptr(AmbientColor));

    // Set light uniforms
    for (int i = 0; i < pointLights.size(); ++i) {
        std::string lightPosName = "PointLightArray[" + std::to_string(i) + "].position";
        std::string lightColorName = "PointLightArray[" + std::to_string(i) + "].color";
        glUniform3fv(glGetUniformLocation(lightingShader, lightPosName.c_str()), 1, glm::value_ptr(pointLights[i].position));
        glUniform3fv(glGetUniformLocation(lightingShader, lightColorName.c_str()), 1, glm::value_ptr(pointLights[i].color));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void DeferredRenderingScene::RenderLightSources() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(lightSourceShader);
    for (int i = 0; i < lightSourceModels.size(); ++i) {

        glUniform3fv(glGetUniformLocation(lightSourceShader, "LightColor"), 1, glm::value_ptr(pointLights[i].color));
        lightSourceModels[i]->Render(lightSourceShader);
    }

    glDisable(GL_BLEND);
}


void DeferredRenderingScene::Render() {
    GeometryPass();
    LightingPass();
    RenderLightSources();

    glfwSwapBuffers(Window);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
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
    delete instanceModel;
    for (auto model : models) {
        delete model;
    }
    for (auto lightSource : lightSourceModels) {
        delete lightSource;
    }

    if (m_FrameBuffer) {
        delete m_FrameBuffer;
        m_FrameBuffer = nullptr;
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}
