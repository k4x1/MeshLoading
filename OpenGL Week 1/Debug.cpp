#include "Debug.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
std::string Debug::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_result;
    localtime_s(&tm_result, &now_time);  
    std::ostringstream oss;
    oss << std::put_time(&tm_result, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}


void Debug::Log(const std::string& message) {
    std::cout << GetTimestamp() << " [LOG] " << message << std::endl;
}

void Debug::LogWarning(const std::string& message) {
    std::cout << GetTimestamp() << " [WARNING] " << message << std::endl;
}

void Debug::LogError(const std::string& message) {
    std::cerr << GetTimestamp() << " [ERROR] " << message << std::endl;
}

void Debug::LogException(const std::exception& ex) {
    std::cerr << GetTimestamp() << " [EXCEPTION] " << ex.what() << std::endl;
}

void Debug::LogAssertion(const std::string& message) {
    std::cerr << GetTimestamp() << " [ASSERTION FAILED] " << message << std::endl;
}
