#include "Debug.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "Camera.h"
std::vector<DebugEntry> Debug::s_entries;
std::mutex              Debug::s_mutex;

std::string Debug::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tmres;
    localtime_s(&tmres, &now_time);
    std::ostringstream oss;
    oss << std::put_time(&tmres, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Debug::AddEntry(DebugLevel lvl, const std::string& msg) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_entries.push_back({ GetTimestamp(), lvl, msg });
    // keep last 1000 entries
    if (s_entries.size() > 1000) s_entries.erase(s_entries.begin());
}

void Debug::Log(const std::string& message) {
    AddEntry(DebugLevel::Info, message);
    std::cout << GetTimestamp() << " [LOG] " << message << "\n";
}

void Debug::LogWarning(const std::string& message) {
    AddEntry(DebugLevel::Warning, message);
    std::cout << GetTimestamp() << " [WARNING] " << message << "\n";
}

void Debug::LogError(const std::string& message) {
    AddEntry(DebugLevel::Error, message);
    std::cerr << GetTimestamp() << " [ERROR] " << message << "\n";
}

void Debug::LogException(const std::exception& ex) {
    AddEntry(DebugLevel::Exception, ex.what());
    std::cerr << GetTimestamp() << " [EXCEPTION] " << ex.what() << "\n";
}

void Debug::LogAssertion(const std::string& message) {
    AddEntry(DebugLevel::Assertion, message);
    std::cerr << GetTimestamp() << " [ASSERTION FAILED] " << message << "\n";
}

const std::vector<DebugEntry>& Debug::GetEntries() {
    return s_entries;
}

void Debug::ClearEntries() {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_entries.clear();
}

void Debug::DrawWireBox(const glm::mat4& M,
    const glm::vec3& h,
    Camera* cam)
{
    glm::vec3 corners[8] = {
      {-h.x,-h.y,-h.z},{ h.x,-h.y,-h.z},
      { h.x, h.y,-h.z},{-h.x, h.y,-h.z},
      {-h.x,-h.y, h.z},{ h.x,-h.y, h.z},
      { h.x, h.y, h.z},{-h.x, h.y, h.z}
    };
    glm::vec4 world[8];
    for (int i = 0; i < 8; i++) world[i] = M * glm::vec4(corners[i], 1.0f);

    glm::mat4 V = cam->GetViewMatrix();
    glm::mat4 P = cam->GetProjectionMatrix();

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadMatrixf(glm::value_ptr(P));
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadMatrixf(glm::value_ptr(V));

    glDisable(GL_DEPTH_TEST);
    glColor3f(1, 1, 0);
    glBegin(GL_LINES);
    auto L = [&](int a, int b) {
        glVertex3fv(glm::value_ptr(glm::vec3(world[a])));
        glVertex3fv(glm::value_ptr(glm::vec3(world[b])));
        };
    L(0, 1); L(1, 2); L(2, 3); L(3, 0);
    L(4, 5); L(5, 6); L(6, 7); L(7, 4);
    for (int i = 0; i < 4; i++) L(i, i + 4);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}