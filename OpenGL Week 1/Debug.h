#pragma once
#include <string>
#include <exception>

class Debug {
public:
    static void Log(const std::string& message);

    static void LogWarning(const std::string& message);

    static void LogError(const std::string& message);

    static void LogException(const std::exception& ex);

    static void LogAssertion(const std::string& message);

private:
    static std::string GetTimestamp();
};
