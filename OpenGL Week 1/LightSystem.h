//#pragma once
//#include <vector>
//#include <glew.h>
//#include <glfw3.h>
//#include <glm/glm.hpp>
//class Scene;
//class DirectionalLightComponent;
//class PointLightComponent;
//class SpotLightComponent;
//
//struct GatheredLights {
//    std::vector<DirectionalLightComponent*> dirs;
//    std::vector<PointLightComponent*>       points;
//    std::vector<SpotLightComponent*>        spots;
//};
//
//class LightSystem {
//public:
//    // Scan the scene once per frame
//    static GatheredLights Collect(Scene* scene);
//
//    // Call right after glUseProgram(shader) but before you draw
//    static void  UploadToShader(GLuint program,
//        const GatheredLights& L,
//        const glm::vec3& cameraPos);
//};
