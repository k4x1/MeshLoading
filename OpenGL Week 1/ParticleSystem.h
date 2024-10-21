#include <glm.hpp>
#include <glew.h>
#include <vector>
#include <gtc/type_ptr.hpp>
#include "ShaderLoader.h"
#include "Camera.h"



class ParticleSystem {
public:
    ParticleSystem(Camera* Cam, GLuint Program_Render, GLuint Program_Compute, glm::vec3 Origin);
    ~ParticleSystem();

    void Update(float DeltaTime);
    void Render();
    bool IsFinished();
    void SetParticleColor(const glm::vec4& color);

private:
    Camera* ActiveCamera;
    GLuint Program_Render;
    GLuint Program_Compute;

    GLuint VAO;
    GLuint VBO_PositionLife;
    GLuint VBO_Velocity;
    GLuint VBO_Color;

    glm::vec3 EmitterOrigin;
    glm::vec4 VelocityLifeChange;

    int GroupCountX;
    int WorkGroupSizeX;
    int NumParticles;

};
