#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>

class ShaderData {
public:
    ShaderData();
    ShaderData(const std::string name, const std::string vertexSource, const std::string fragmentSource);
    std::string name;
    std::string vertexSource;
    std::string fragmentSource;
};

class ShaderLoader{
public:
    ShaderLoader();
    size_t loadShaderData(const std::string& filePath);
    std::string getShaderName(size_t ShaderHashKey);
    ShaderData* getShader(size_t ShaderHashKey);
    std::vector<std::pair<std::string,size_t>> getShaderList();
private:
    std::unordered_map<size_t,ShaderData> myShaderSources;
};