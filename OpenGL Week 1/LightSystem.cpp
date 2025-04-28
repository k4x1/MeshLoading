//#include "LightSystem.h"
//#include "Scene.h"
//#include "Light.h"          
//#include <glm/gtc/type_ptr.hpp>
//#include <string>
//GatheredLights LightSystem::Collect(Scene* scene) {
//    GatheredLights L;
//    for (auto* go : scene->gameObjects) {
//        for (auto& comp : go->components) {
//            if (auto* dl = dynamic_cast<DirectionalLightComponent*>(comp.get()))
//                L.dirs.push_back(dl);
//            else if (auto* pl = dynamic_cast<PointLightComponent*>(comp.get()))
//                L.points.push_back(pl);
//            else if (auto* sl = dynamic_cast<SpotLightComponent*>(comp.get()))
//                L.spots.push_back(sl);
//        }
//    }
//    return L;
//}
//
//
//void LightSystem::UploadToShader(GLuint program,
//    const GatheredLights& L,
//    const glm::vec3& camPos)
//{
//    glUseProgram(program);
//
//    if (GLint loc = glGetUniformLocation(program, "CameraPos"); loc >= 0)
//        glUniform3fv(loc, 1, glm::value_ptr(camPos));
//
//    if (!L.dirs.empty()) {
//        auto* D = L.dirs.front();
//        if (GLint loc = glGetUniformLocation(program, "DirLight.direction"); loc >= 0)
//            glUniform3fv(loc, 1, glm::value_ptr(D->direction));
//        if (GLint loc = glGetUniformLocation(program, "DirLight.color"); loc >= 0)
//            glUniform3fv(loc, 1, glm::value_ptr(D->color));
//        if (GLint loc = glGetUniformLocation(program, "DirLight.specularStrength"); loc >= 0)
//            glUniform1f(loc, D->specularStrength);
//    }
//
//    GLuint count = (GLuint)L.points.size();
//    if (GLint loc = glGetUniformLocation(program, "PointLightCount"); loc >= 0)
//        glUniform1ui(loc, count);
//
//    for (GLuint i = 0; i < count; ++i) {
//        auto* P = L.points[i];
//        const std::string prefix = "PointLightArray[" + std::to_string(i) + "].";
//
//        auto set3 = [&](const char* name, const glm::vec3& v) {
//            if (GLint loc = glGetUniformLocation(program, (prefix + name).c_str()); loc >= 0)
//                glUniform3fv(loc, 1, glm::value_ptr(v));
//            };
//        auto set1 = [&](const char* name, float f) {
//            if (GLint loc = glGetUniformLocation(program, (prefix + name).c_str()); loc >= 0)
//                glUniform1f(loc, f);
//            };
//
//        set3("position", P->position);
//        set3("color", P->color);
//        set1("specularStrength", P->specularStrength);
//        set1("attenuationConstant", P->attenuationConstant);
//        set1("attenuationLinear", P->attenuationLinear);
//        set1("attenuationExponent", P->attenuationExponent);
//    }
//
//    if (!L.spots.empty()) {
//        auto* S = L.spots.front();
//        const std::string p = "SptLight.";
//
//        auto set3 = [&](const char* name, const glm::vec3& v) {
//            if (GLint loc = glGetUniformLocation(program, (p + name).c_str()); loc >= 0)
//                glUniform3fv(loc, 1, glm::value_ptr(v));
//            };
//        auto set1 = [&](const char* name, float f) {
//            if (GLint loc = glGetUniformLocation(program, (p + name).c_str()); loc >= 0)
//                glUniform1f(loc, f);
//            };
//
//        set3("position", S->position);
//        set3("direction", S->direction);
//        set3("color", S->color);
//        set1("specularStrength", S->specularStrength);
//        set1("attenuationConstant", S->attenuationConstant);
//        set1("attenuationLinear", S->attenuationLinear);
//        set1("attenuationExponent", S->attenuationExponent);
//        set1("innerCutoff", S->innerCutoff);
//        set1("outerCutoff", S->outerCutoff);
//    }
//}