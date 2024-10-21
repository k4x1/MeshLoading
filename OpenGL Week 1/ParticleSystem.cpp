#include "ParticleSystem.h"
ParticleSystem::ParticleSystem(Camera* Cam, GLuint Program_Render, GLuint Program_Compute, glm::vec3 Origin) {
    this->ActiveCamera = Cam;
    this->Program_Render = Program_Render;
    this->Program_Compute = Program_Compute;
    this->EmitterOrigin = Origin;

    GroupCountX = 10;
    WorkGroupSizeX = 128; 
    NumParticles = WorkGroupSizeX * GroupCountX;
    // Generate buffers
    glGenBuffers(1, &VBO_PositionLife);
    glGenBuffers(1, &VBO_Velocity);
    glGenBuffers(1, &VBO_Color);

    // Allocate storage for buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PositionLife);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * NumParticles, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Velocity);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * NumParticles, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * NumParticles, NULL, GL_DYNAMIC_DRAW);

    // Create VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Bind VBO_PositionLife to binding point 0
    glBindVertexBuffer(0, VBO_PositionLife, 0, sizeof(glm::vec4));
    glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);
    glEnableVertexAttribArray(0);

    // Bind VBO_Velocity to binding point 1
    glBindVertexBuffer(1, VBO_Velocity, 0, sizeof(glm::vec4));
    glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);
    glEnableVertexAttribArray(1);

    // Bind VBO_Color to binding point 2
    glBindVertexBuffer(2, VBO_Color, 0, sizeof(glm::vec4));
    glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(2, 2);
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);

}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(float DeltaTime) {
    // Calculate gravity effect and velocity life change
    glm::vec3 Gravity = glm::vec3(0.0f, -9.8f, 0.0f) * DeltaTime;
    VelocityLifeChange = glm::vec4(Gravity, DeltaTime);


}


void ParticleSystem::Render() {

    glUseProgram(Program_Compute);
    // Compute pass

    // Bind the position/lifespan buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO_PositionLife);

    // Bind the velocity buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VBO_Velocity);

    // Set the VelocityLifeChange uniform

    glUniform4fv(glGetUniformLocation(Program_Compute, "VelocityLifeChange"), 1, glm::value_ptr(VelocityLifeChange));

    // Set the EmitterOrigin uniform
    glUniform3fv(glGetUniformLocation(Program_Compute, "EmitterOrigin"), 1, glm::value_ptr(EmitterOrigin));

    // Set random seed uniforms
    glUniform1i(glGetUniformLocation(Program_Compute, "SeedLife"), rand());
    glUniform1i(glGetUniformLocation(Program_Compute, "SeedX"), rand());
    glUniform1i(glGetUniformLocation(Program_Compute, "SeedY"), rand());
    glUniform1i(glGetUniformLocation(Program_Compute, "SeedZ"), rand());
    // Dispatch the compute shader
    glDispatchCompute(GroupCountX, 1, 1);

    // Ensure all writes to the buffers are completed
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);


    // Render pass
    glUseProgram(Program_Render);
    glBindVertexArray(VAO);

    glm::mat4 viewMatrix = ActiveCamera->GetViewMatrix();
    glm::mat4 projectionMatrix = ActiveCamera->GetProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(Program_Render, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(Program_Render, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);

    glDrawArrays(GL_POINTS, 0, NumParticles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
   
 //   glDisable(GL_BLEND);
}
