#pragma once

#include <string>
#include <exception>
#include <vector>
#include <mutex>
#include <source_location>
#include <glm.hpp> 
#include <sstream>
#include <initializer_list>
#include "EnginePluginAPI.h"
class Camera;                  

enum class ENGINE_API DebugLevel { Log, Warning, Error, Exception };

struct ENGINE_API DebugEntry {
    std::string timestamp;
    DebugLevel  level = DebugLevel::Log;
    std::string message;
    std::string file;
    int         line = 0;
    std::string function;
};
class ENGINE_API Debug {
public:
    static void LogImpl(const std::string& message,
        const std::source_location& location = std::source_location::current());

    static void LogWarningImpl(const std::string& message,
        const std::source_location& location = std::source_location::current());

    static void LogErrorImpl(const std::string& message,
        const std::source_location& location = std::source_location::current());

    static void LogExceptionImpl(const std::string& message,
        const std::source_location& location = std::source_location::current());


    static void DrawWireBox(
        const glm::mat4& model,
        const glm::vec3& halfExtents,
        Camera* cam);

    static void DrawRay(
        const glm::vec3& origin,
        const glm::vec3& dir,
        float length,
        Camera* cam,
        const glm::vec4& color = glm::vec4(0, 1, 0, 1)  
    );
    static const std::vector<DebugEntry>& GetEntries();
    static void ClearEntries();

private:
    static std::string GetTimestamp();
    static void AddEntry(DebugLevel lvl, const std::string& msg, const std::source_location& loc = std::source_location::current());
      
        

    static std::vector<DebugEntry> s_entries;
    static std::mutex              s_mutex;
};

#define DEBUG_LOG(...)\
  do {\
    std::ostringstream _dbg_oss;\
    (_dbg_oss << __VA_ARGS__);\
    Debug::LogImpl(_dbg_oss.str(), std::source_location::current());\
  } while(0)

#define DEBUG_ERR(...)\
  do {\
    std::ostringstream _dbg_oss;\
    (_dbg_oss << __VA_ARGS__);\
    Debug::LogErrorImpl(_dbg_oss.str(), std::source_location::current());\
  } while(0)


#define DEBUG_WARN(...)\
  do {\
    std::ostringstream _dbg_oss;\
    (_dbg_oss << __VA_ARGS__);\
    Debug::LogWarningImpl(_dbg_oss.str(), std::source_location::current());\
  } while(0)


#define DEBUG_EXC(...)\
  do {\
    std::ostringstream _dbg_oss\
    (_dbg_oss << __VA_ARGS__)\
    Debug::LogExceptionImpl(_dbg_oss.str(), std::source_location::current());\
  } while(0)
