#include "ParticleScene.h"
#include "HeightMapInfo.h"
#include <iostream>

void ParticleScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    Window = _window;
    camera = _camera;
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
    shadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMapping.vert", "Resources/Shaders/ShadowMapping.frag");
    instanceShadowMappingShader = ShaderLoader::CreateProgram("Resources/Shaders/ShadowMappingInstance.vert", "Resources/Shaders/ShadowMapping.frag");
    mainRenderingShader = ShaderLoader::CreateProgram("Resources/Shaders/MainRendering.vert", "Resources/Shaders/MainRendering.frag");
    // Load compute shader
    Program_ComputeParticles = ShaderLoader::CreateComputeProgram("Resources/Shaders/ComputeParticles.comp");

    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    Program_particle = ShaderLoader::CreateProgram("Resources/Shaders/Particle.vert", "Resources/Shaders/Particle.frag");


    InitializeModels();
    SetupLights();
    SetupTerrain();

    // Initialize FrameBuffer
    m_FrameBuffer = new FrameBuffer(800, 800);

    SetupQuad();
    particleSystem = new ParticleSystem(camera, Program_particle, Program_ComputeParticles, glm::vec3(0.0f, 0.0f, 0.0f));
}

void ParticleScene::Update()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // circle parameters
    float radius = 1000.0f;
    float speed = 1.0f;
    glm::vec3 center(0.0f, 500.0f, 0.0f);

    //  new position
    float angle = speed * currentFrame;
    float x = center.x + radius * cos(angle);
    float z = center.z + radius * sin(angle);

    // Update model position
    model->SetPosition(glm::vec3(x, center.y, z));

    // Update model rotation to face the direction of movement (optional)
    float rotationAngle = glm::degrees(atan2(-sin(angle), -cos(angle)));
    model->SetRotation(glm::vec3(0, rotationAngle, 0));

    // Update other scene elements
    model->Update(deltaTime);

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

    // Update Particle System


    // Reinitialize particles on 'F' key press
    if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS) {
        delete particleSystem;
        particleSystem = new ParticleSystem(camera, Program_particle, Program_ComputeParticles, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}
void ParticleScene::Render() {

    RenderShadowMap(m_ShadowMap1, dirLight1);
    RenderShadowMap(m_ShadowMap2, dirLight2);
    RenderSceneWithShadows();

   // Check for OpenGL errors
   GLenum error = glGetError();
   if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
    particleSystem->Update(deltaTime);
    particleSystem->Render();
    //RenderPostProcessing();
}

void ParticleScene::RenderShadowMap(ShadowMap* shadowMap, const DirectionalLight& dirLight) {
    shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::vec3 lightPos = dirLight.direction * -700.0f;
    shadowMap->UpdateLightSpaceMatrix(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    glUseProgram(shadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(shadowMap->GetLightSpaceMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader, "model"), 1, GL_FALSE, glm::value_ptr(model->m_modelMatrix));
    model->Render(shadowMappingShader);

    glUseProgram(instanceShadowMappingShader);
    glUniformMatrix4fv(glGetUniformLocation(instanceShadowMappingShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(shadowMap->GetLightSpaceMatrix()));
    instanceModel->Render(instanceShadowMappingShader);

    shadowMap->Unbind();
}

void ParticleScene::RenderSceneWithShadows() {
    m_FrameBuffer->Bind();
    glViewport(0, 0, 800, 800);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);

    glUseProgram(model->GetShader());
    model->BindTexture();
    model->PassUniforms(camera);
    model->PassDirectionalUniforms(dirLight1);
    model->PassSpotLightUniforms(spotLight);

    glActiveTexture(GL_TEXTURE2);
    m_ShadowMap1->BindTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(model->GetShader(), "shadowMap1"), 2);
    glUniformMatrix4fv(glGetUniformLocation(model->GetShader(), "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));

    glActiveTexture(GL_TEXTURE3);
    m_ShadowMap2->BindTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(model->GetShader(), "shadowMap2"), 3);
    glUniformMatrix4fv(glGetUniformLocation(model->GetShader(), "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_Texture, "skybox"), 1);

    model->Render(model->GetShader());

    glUseProgram(instanceModel->GetShader());
    instanceModel->BindTexture();
    instanceModel->PassUniforms(camera);
    instanceModel->PassDirectionalUniforms(dirLight1);
    instanceModel->PassSpotLightUniforms(spotLight);

    glActiveTexture(GL_TEXTURE2);
    m_ShadowMap1->BindTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(instanceModel->GetShader(), "shadowMap1"), 2);
    glUniformMatrix4fv(glGetUniformLocation(instanceModel->GetShader(), "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));

    glActiveTexture(GL_TEXTURE3);
    m_ShadowMap2->BindTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(instanceModel->GetShader(), "shadowMap2"), 3);
    glUniformMatrix4fv(glGetUniformLocation(instanceModel->GetShader(), "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_instanceTexture, "skybox"), 1);
    glUniform1i(glGetUniformLocation(instanceModel->GetShader(), "Texture0"), 0);
    instanceModel->Render(instanceModel->GetShader());

    glUseProgram(Program_terrain);
    glBindVertexArray(terrainVAO);

    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "view"), 1, GL_FALSE, glm::value_ptr(camera->m_view));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(camera->m_projection));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix1"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap1->GetLightSpaceMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(Program_terrain, "lightSpaceMatrix2"), 1, GL_FALSE, glm::value_ptr(m_ShadowMap2->GetLightSpaceMatrix()));

    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos1"), 1, glm::value_ptr(dirLight1.direction * -700.0f));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightPos2"), 1, glm::value_ptr(dirLight2.direction * -700.0f));
    glUniform3fv(glGetUniformLocation(Program_terrain, "viewPos"), 1, glm::value_ptr(camera->m_position));
    glUniform3fv(glGetUniformLocation(Program_terrain, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(glGetUniformLocation(Program_terrain, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

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

    // Bind shadow maps
    glActiveTexture(GL_TEXTURE4);
    m_ShadowMap1->BindTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(Program_terrain, "shadowMap1"), 4);

    glActiveTexture(GL_TEXTURE5);
    m_ShadowMap2->BindTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(Program_terrain, "shadowMap2"), 5);

    // Draw the terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    m_FrameBuffer->Unbind();

    // Third pass: Post-processing
    glViewport(0, 0, 800, 800);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessingShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcessingShader, "effect"), static_cast<int>(currentEffect));

    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    m_FrameBuffer->BindTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);


    // Swap buffers

}
void ParticleScene::RenderPostProcessing() {
    // Set the viewport to the window size
    glViewport(0, 0, 800, 800);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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


void ParticleScene::InitializeModels() {
    glm::vec3 position(0.0f, 500.0f, 0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(0.05f);

    model = new MeshModel(position, rotation, scale, "Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    model->SetTexture(ancientTex.GetId());
    model->SetShader(Program_Texture);

    faces = {
        "Resources/Textures/skybox/Right.png",
        "Resources/Textures/skybox/Left.png",
        "Resources/Textures/skybox/Bottom.png",
        "Resources/Textures/skybox/Top.png",
        "Resources/Textures/skybox/Back.png",
        "Resources/Textures/skybox/Front.png"
    };
    skybox = new Skybox("Resources/Models/cube.obj", faces);

    instanceModel = new InstanceMeshModel(glm::vec3(0), glm::vec3(0), glm::vec3(500.0f), "Resources/Models/AncientEmpire/SM_Env_Tree_Palm_01.obj");
    instanceModel->SetTexture(ancientTex.GetId());
    instanceModel->SetShader(Program_instanceTexture);

    for (int i = 0; i < 100; i++) {
        glm::vec3 randomPosition = glm::vec3(rand() % 300 - 150, 10.0f, rand() % 300 - 150);
        glm::vec3 randomRotation = glm::vec3((rand() % 21) - 10);
        glm::vec3 randomScale = glm::vec3(((rand() % 10 + 5) / 200.0f));
        instanceModel->AddInstance(randomPosition, randomRotation, randomScale);
    }
    instanceModel->initBuffer();
}
void ParticleScene::SetupLights() {
    m_ShadowMap1 = new ShadowMap();
    m_ShadowMap2 = new ShadowMap();

    dirLight1.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight1.color = glm::vec3(1.0f, 1.0f, 1.0f);

    dirLight2.direction = glm::vec3(0.3f, -1.0f, 0.2f);
    dirLight2.color = glm::vec3(0.8f, 0.8f, 0.8f);

    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;
    spotLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specularStrength = 1.0f;
    spotLight.attenuationConstant = 1.0f;
    spotLight.attenuationLinear = 0.014f;
    spotLight.attenuationExponent = 0.0007f;
    spotLight.innerCutoff = glm::cos(glm::radians(13.0f));
    spotLight.outerCutoff = glm::cos(glm::radians(15.0f));



    glDisable(GL_CULL_FACE);
}
void ParticleScene::SetupTerrain() {
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
void ParticleScene::SetupQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
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

void ParticleScene::InitializeParticles() {
    // Initialize particles with random positions, velocities, and colors
    for (int i = 0; i < 1000; ++i) {
        glm::vec3 position(0.0f, 0.0f, 0.0f);
        glm::vec3 velocity(0.0f, 10.0f, 0.0f); // Upward velocity
        glm::vec4 color(static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX, 1.0f);
        float lifetime = static_cast<float>(rand()) / RAND_MAX * 5.0f; // Random lifetime
        particles.emplace_back(position, velocity, color, lifetime);
    }

    // Load particle shader
    particleShaderProgram = ShaderLoader::CreateProgram("Resources/Shaders/Particle.vert", "Resources/Shaders/Particle.frag");
}

void ParticleScene::UpdateParticles() {
    for (auto& particle : particles) {
        particle.Update(deltaTime);
    }
    // Remove particles that have expired
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.lifetime <= 0.0f; }),
        particles.end());
}

void ParticleScene::RenderParticles() {
    glUseProgram(particleShaderProgram);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (const auto& particle : particles) {
        particle.Render();
    }
    glDisable(GL_BLEND);
}

int ParticleScene::MainLoop() {
    while (!glfwWindowShouldClose(Window)) {
        Update();
        Render();
        glfwPollEvents();
    }
    return 0;
}
ParticleScene::~ParticleScene() {
    delete model;
    delete skybox;
    delete instanceModel;
    delete pointLight1;
    delete pointLight2;

    if (m_FrameBuffer) {
        delete m_FrameBuffer;
        m_FrameBuffer = nullptr;
    }

    if (particleSystem) {
        delete particleSystem;
        particleSystem = nullptr;
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}