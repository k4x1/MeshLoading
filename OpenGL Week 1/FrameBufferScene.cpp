
#include "FrameBufferScene.h"
#include <iostream>

void FrameBufferScene::InitialSetup(GLFWwindow* _window, Camera* _camera)
{
    Window = _window;
    camera = _camera;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 800);
    //  inputs = new InputManager(camera);

     // inputs->SetCursorPosCallback(Window);
      // Initialize camera
    camera->InitCamera(800, 800, glm::vec3(0.0f, 0.0f, 10.0f));

    // Load shaders

    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    Program_instanceTexture = ShaderLoader::CreateProgram("Resources/Shaders/InstanceTexture.vert", "Resources/Shaders/InstanceTexture.frag");
    Program_color = ShaderLoader::CreateProgram("Resources/Shaders/Color.vert", "Resources/Shaders/Color.frag");
    postProcessingShader = ShaderLoader::CreateProgram("Resources/Shaders/Quad.vert", "Resources/Shaders/PostProcessing.frag");

    // Initialize textures
    ancientTex.InitTexture("Resources/Textures/PolygonAncientWorlds_Texture_01_A.png");
    scifiTex.InitTexture("Resources/Textures/PolygonScifiWorlds_Texture_01_B.png");
    skyboxTex.InitTexture("Resources/Textures/fakerRomance.png");
    blankTex.InitTexture("Resources/Textures/blankTex.png");

    // Initialize models
    glm::vec3 position(0.0f, -100.0f, 0.0f);
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

    // Add instances at random positions
    for (int i = 0; i < 1000; i++) {
        glm::vec3 randomPosition = glm::vec3(rand() % 1000 - 100, -5.0f, rand() % 1000 - 100);
        glm::vec3 randomRotation = glm::vec3((rand() % 21) - 10);
        glm::vec3 randomScale = glm::vec3(((rand() % 10 + 5) / 200.0f));
        instanceModel->AddInstance(randomPosition, randomRotation, randomScale);
    }
    instanceModel->initBuffer();



    PointLightArray[0].position = glm::vec3(25.0f, 15.0f, 0.0f);
    PointLightArray[0].color = glm::vec3(0.0f, 0.0f, 1.0f);
    PointLightArray[0].specularStrength = 2.0f;
    PointLightArray[0].attenuationConstant = 1.0f;
    PointLightArray[0].attenuationLinear = 0.045f;
    PointLightArray[0].attenuationExponent = 0.0075f;



    PointLightArray[1].position = glm::vec3(-25.0f, 15.0f, 0.0f);
    PointLightArray[1].color = glm::vec3(1.0f, 0.0f, 0.0f);
    PointLightArray[1].specularStrength = 2.0f;
    PointLightArray[1].attenuationConstant = 1.0f;
    PointLightArray[1].attenuationLinear = 0.045f;
    PointLightArray[1].attenuationExponent = 0.0075f;

    PointLightCount = 2;
    // Directional light
    dirLight.direction = glm::vec3(-1.0f, -1.0f, 0.0f);
    dirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    dirLight.specularStrength = 1.0f;


    pointLight1 = new MeshModel(PointLightArray[0].position, glm::vec3(0), glm::vec3(1), "Resources/Models/Sphere.obj");
    pointLight1->SetTexture(blankTex.GetId());
    pointLight1->SetShader(Program_color);

    pointLight2 = new MeshModel(PointLightArray[1].position, glm::vec3(0), glm::vec3(1), "Resources/Models/Sphere.obj");
    pointLight2->SetTexture(blankTex.GetId());
    pointLight2->SetShader(Program_color);

    spotLight.position = camera->m_position;
    spotLight.direction = camera->m_orientation;
    spotLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specularStrength = 1.0f;
    spotLight.attenuationConstant = 1.0f;
    spotLight.attenuationLinear = 0.014f;
    spotLight.attenuationExponent = 0.0007f;
    spotLight.innerCutoff = glm::cos(glm::radians(13.0f));
    spotLight.outerCutoff = glm::cos(glm::radians(15.0f));

    // Set up framebuffer
    m_FrameBuffer = new FrameBuffer(800, 800);
   

    // Set up quad VAO
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

void FrameBufferScene::Update()
{
    //  inputs->ProcessInput(Window);
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
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
}


void FrameBufferScene::Render()
{
    // Clear the color buffer
    m_FrameBuffer->Bind();
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GLenum error = glGetError();
    // Render the skybox
    glm::mat4 view = glm::mat4(glm::mat3(camera->m_view));
    camera->Matrix(0.01f, 1000.0f);
    glm::mat4 projection = camera->m_projection;
    skybox->Render(view, projection);


    glUseProgram(model->GetShader());
    model->BindTexture();
    model->PassUniforms(camera);
    model->PassPointUniforms(camera, PointLightArray, PointLightCount);
    model->PassDirectionalUniforms(dirLight);
    model->PassSpotLightUniforms(spotLight);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_Texture, "skybox"), 1);

    model->Render(model->GetShader());
    glUseProgram(instanceModel->GetShader());

    instanceModel->BindTexture();
    instanceModel->PassUniforms(camera);
    instanceModel->PassPointUniforms(camera, PointLightArray, PointLightCount);
    instanceModel->PassDirectionalUniforms(dirLight);
    instanceModel->PassSpotLightUniforms(spotLight);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
    glUniform1i(glGetUniformLocation(Program_instanceTexture, "skybox"), 1);
    glUniform1i(glGetUniformLocation(instanceModel->GetShader(), "Texture0"), 0);
    instanceModel->Render(instanceModel->GetShader());




    // Render instance model
    glUseProgram(pointLight1->GetShader());
    pointLight1->BindTexture();
    pointLight1->PassUniforms(camera);
    pointLight1->PassColorUniforms(PointLightArray[0].color.r, PointLightArray[0].color.g, PointLightArray[0].color.b, 1.0f);
    pointLight1->Render(pointLight1->GetShader());

    glUseProgram(pointLight2->GetShader());
    pointLight2->BindTexture();
    pointLight2->PassUniforms(camera);
    pointLight2->PassColorUniforms(PointLightArray[1].color.r, PointLightArray[1].color.g, PointLightArray[1].color.b, 1.0f);
    pointLight2->Render(pointLight2->GetShader());



    m_FrameBuffer->Unbind();

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


    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
    // Check for OpenGL errors
    // Swap buffers
    
}
int FrameBufferScene::MainLoop() {
    return 0;
}
FrameBufferScene::~FrameBufferScene() {
    delete model;
    delete skybox;
    delete instanceModel;
    delete pointLight1;
    delete pointLight2;

    delete m_FrameBuffer;
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}
