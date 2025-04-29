#pragma once

#include <string>
#include <exception>
#include <vector>
#include <mutex>
#include <glm.hpp>      
#include "EnginePluginAPI.h"
class Camera;                  

enum class ENGINE_API DebugLevel { Info, Warning, Error, Exception, Assertion };

struct ENGINE_API DebugEntry {
    std::string timestamp;
    DebugLevel  level = DebugLevel::Info;
    std::string message;
};

class ENGINE_API Debug {
public:
    static void Log(const std::string& message);
    static void LogWarning(const std::string& message);
    static void LogError(const std::string& message);
    static void LogException(const std::exception& ex);
    static void LogAssertion(const std::string& message);

    template<typename T>
    static void Log(const T& value) {
        std::ostringstream oss;
        oss << value;
        Log(oss.str());
    }

    template<typename T>
    static void LogWarning(const T& value) {
        std::ostringstream oss;
        oss << value;
        LogWarning(oss.str());
    }

    template<typename T>
    static void LogError(const T& value) {
        std::ostringstream oss;
        oss << value;
        LogError(oss.str());
    }

    template<typename T>
    static void LogAssertion(const T& value) {
        std::ostringstream oss;
        oss << value;
        LogAssertion(oss.str());
    }

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
