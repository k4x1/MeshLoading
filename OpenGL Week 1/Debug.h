#pragma once

#include <string>
#include <exception>
#include <vector>
#include <mutex>
#include <glm/glm.hpp>      

class Camera;                  

enum class DebugLevel { Info, Warning, Error, Exception, Assertion };

struct DebugEntry {
    std::string timestamp;
    DebugLevel  level;
    std::string message;
};

class Debug {
public:
    static void Log(const std::string& message);
    static void LogWarning(const std::string& message);
    static void LogError(const std::string& message);
    static void LogException(const std::exception& ex);
    static void LogAssertion(const std::string& message);

    static void DrawWireBox(
        const glm::mat4& model,
        const glm::vec3& halfExtents,
        Camera* cam);

    static const std::vector<DebugEntry>& GetEntries();
    static void ClearEntries();

private:
    static std::string GetTimestamp();
    static void    AddEntry(DebugLevel lvl, const std::string& msg);

    static std::vector<DebugEntry> s_entries;
    static std::mutex              s_mutex;
};
