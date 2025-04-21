#pragma once
#include <string>
#include <map>
#include <utility>
#include <glew.h>
#include "ShaderLoader.h"
class ShaderManager {
public:
    static ShaderManager& Instance() {
        static ShaderManager inst;
        return inst;
    }

    // compile once and cache; subsequent calls return cached program
    GLuint GetShader(const std::string& vertPath,
        const std::string& fragPath)
    {
        auto key = std::make_pair(vertPath, fragPath);
        auto it = _cache.find(key);
        if (it != _cache.end())
            return it->second;

        // first time: compile via your existing ShaderLoader
        GLuint prog = ShaderLoader::CreateProgram(vertPath.c_str(),
            fragPath.c_str());
        _cache[key] = prog;
        return prog;
    }

private:
    ShaderManager() = default;
    ~ShaderManager() {
        // optional: glDeleteProgram for each cached program
    }

    // no copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::map<std::pair<std::string, std::string>, GLuint> _cache;
};
