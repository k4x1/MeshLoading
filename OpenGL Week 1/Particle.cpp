#include "Particle.h"



Particle::Particle(const glm::vec3& _startPos, const glm::vec3& _startVel, const glm::vec4& _startColor, float _startLifetime) : position(_startPos), velocity(_startVel), color(_startColor), lifetime(_startLifetime), startLifetime(_startLifetime)
{
}
void Particle::Update(float deltaTime)
{
    // Update position based on velocity
    position += velocity * deltaTime;

    // Decrease lifetime
    lifetime -= deltaTime;

    // Fade out the particle
    color.a = lifetime > 0 ? lifetime / startLifetime : 0.0f;
}

void Particle::Render() const
{
    // Set the color for the particle
    glColor4f(color.r, color.g, color.b, color.a);

    // Render the particle as a point
    glBegin(GL_POINTS);
    glVertex3f(position.x, position.y, position.z);
    glEnd();
}