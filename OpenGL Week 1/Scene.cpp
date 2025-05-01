#include "Scene.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "PhysicsEngine.h"
#include <reactphysics3d/utils/DebugRenderer.h>

void Scene::InitialSetup(GLFWwindow* _window, bool autoLoad) {
    Window = _window;
}

void Scene::Start()
{
    for (GameObject* obj : gameObjects) {
        obj->Start();
    }
}

void Scene::Update(float dt) {
    for (GameObject* obj : gameObjects) {

        obj->Update(dt);
    }
}
void Scene::FixedUpdate(float fixedDt) {

    Physics::PhysicsEngine::Instance().FixedUpdate(fixedDt);
    for (auto* go : gameObjects)
        go->FixedUpdate(fixedDt);
}

void Scene::Render(FrameBuffer* currentBuffer, Camera* _camera) {
    Camera* renderCamera = (_camera != nullptr) ? _camera : camera;
    for (GameObject* obj : gameObjects) {
        obj->Render(renderCamera);
    }
}

void Scene::DrawGizmos(Camera* cam)
{
    
    auto& debugRef = Physics::PhysicsEngine::Instance().GetWorld()->getDebugRenderer();
    debugRef.reset();
    debugRef.computeDebugRenderingPrimitives(*Physics::PhysicsEngine::Instance().GetWorld());
    for (auto* go : gameObjects) {
        for (auto& comp : go->components) {
            comp->OnDrawGizmos(cam);
        }
    }
    rp3d::uint32 nbLines = debugRef.getNbLines();
    if (nbLines > 0) {
        const auto* lines = debugRef.getLinesArray();
        for (rp3d::uint32 i = 0; i < nbLines; ++i) {
            const auto& L = lines[i];
            glm::vec3 p1{ L.point1.x, L.point1.y, L.point1.z };
            glm::vec3 p2{ L.point2.x, L.point2.y, L.point2.z };

            auto unpack = [&](rp3d::uint32 c) {
                return glm::vec4(
                    float((c >> 16) & 0xFF) / 255.0f,
                    float((c >> 8) & 0xFF) / 255.0f,
                    float((c) & 0xFF) / 255.0f,
                    1.0f
                );
                };

            glm::vec4 col = unpack(L.color1);
            //DEBUG_LOG("drawing line start ["<<p1 << " | " << p2 << "]");
            Debug::DrawLine(p1, p2, cam, col);
        }
    }
}

FrameBuffer* Scene::GetFrameBuffer()
{
    return gameFrameBuffer;
}

void Scene::AddGameObject(GameObject* obj) {
    gameObjects.push_back(obj);
}

void Scene::RemoveGameObject(GameObject* obj) {
    if (obj->parent) {
        auto& sib = obj->parent->children;
        sib.erase(std::remove(sib.begin(), sib.end(), obj), sib.end());
    }
    gameObjects.erase(
        std::remove(gameObjects.begin(), gameObjects.end(), obj),
        gameObjects.end()
    );
    delete obj;
}


nlohmann::json Scene::ToJson() const {
    nlohmann::json j;
    j["gameObjects"] = nlohmann::json::array();
    for (auto* go : gameObjects)
        j["gameObjects"].push_back(SerializeGameObject(go));
    return j;
}

void Scene::FromJson(const nlohmann::json& j) {
    for (auto* go : gameObjects)
    {
        delete go;
        gameObjects.clear();
    }

    for (auto& objJ : j["gameObjects"])
        gameObjects.push_back(DeserializeGameObject(objJ));
}

std::string Scene::ToString() const {
    return ToJson().dump();
}

void Scene::FromString(const std::string& s) {
    FromJson(nlohmann::json::parse(s));
}
void Scene::SaveToFile(const std::string& fileName) {
    namespace fs = std::filesystem;
    fs::path outPath = fs::path("Assets") / fileName;

    nlohmann::json j;
    j["gameObjects"] = nlohmann::json::array();
    for (GameObject* obj : gameObjects) {
        if (obj->parent == nullptr) {
            j["gameObjects"].push_back(SerializeGameObject(obj));
        }
    }

    std::ofstream outFile(outPath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for saving: " << outPath << std::endl;
        return;
    }
    outFile << j.dump(4);
}

void Scene::LoadFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open scene file: " << filePath << std::endl;
        return;
    }
    nlohmann::json j;
    inFile >> j;
    inFile.close();

    for (auto* obj : gameObjects) delete obj;
    gameObjects.clear();

    if (j.contains("gameObjects")) {
        for (auto& objJson : j["gameObjects"]) {
            GameObject* root = DeserializeGameObject(objJson);
            gameObjects.push_back(root);

            std::function<void(GameObject*)> addDescendants = [&](GameObject* parent) {
                for (GameObject* child : parent->children) {
                    gameObjects.push_back(child);
                    addDescendants(child);
                }
                };
            addDescendants(root);
        }
    }
    std::cout << "Loaded file from: " << filePath << std::endl;
}

Scene::~Scene() {
    for (GameObject* obj : gameObjects) {
        if (obj && obj->parent == nullptr) {
            delete obj;
        }
    }
    gameObjects.clear();
}
