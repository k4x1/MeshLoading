#include <glm.hpp>
#include <glew.h>

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float lifetime;

    Particle(const glm::vec3& _startPos, const glm::vec3& _startVel, const glm::vec4& _startColor, float _startLifetime);

    void Update(float _deltaTime);

    void Render() const;

private:
    float startLifetime = 0; // Store the initial lifetime for fading calculations
};
