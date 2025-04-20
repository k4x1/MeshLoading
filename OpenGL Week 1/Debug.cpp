#include "Debug.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

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
