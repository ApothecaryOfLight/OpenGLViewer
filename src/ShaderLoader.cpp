#include <iostream>
#include <tinyxml2.h>

#include "ShaderLoader.hpp"
#include <string>
#include <unordered_map>

ShaderData::ShaderData() {

}

ShaderData::ShaderData(std::string inName, std::string inVertexSource, std::string inFragmentSource) {
    name.assign(inName);
    vertexSource.assign(inVertexSource);
    fragmentSource.assign(inFragmentSource);
}

ShaderLoader::ShaderLoader() {
    std::cout << "ShaderLoader constructor called!" << std::endl;
}

size_t ShaderLoader::loadShaderData(const std::string& filePath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
        return false;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("Shader");
    if (!root) {
        std::cerr << "Missing <Shader> element in XML file." << std::endl;
        return false;
    }

    tinyxml2::XMLElement* nameElement = root->FirstChildElement("name");
    tinyxml2::XMLElement* vertexElement = root->FirstChildElement("vertexSource");
    tinyxml2::XMLElement* fragmentElement = root->FirstChildElement("fragmentSource");

    if (nameElement && nameElement->GetText()) {
    } else {
        std::cerr << "Missing or empty <name> element." << std::endl;
        return false;
    }

    if (vertexElement && vertexElement->GetText()) {
    } else {
        std::cerr << "Missing or empty <vertexSource> element." << std::endl;
        return false;
    }

    if (fragmentElement && fragmentElement->GetText()) {
    } else {
        std::cerr << "Missing or empty <fragmentSource> element." << std::endl;
        return false;
    }

    size_t shader_hash_key = std::hash<std::string>{}(nameElement->GetText());
    myShaderSources.emplace(shader_hash_key, ShaderData(nameElement->GetText(), vertexElement->GetText(), fragmentElement->GetText()));

    return shader_hash_key;
}

std::string ShaderLoader::getShaderName(size_t ShaderHashKey) {
    return myShaderSources[ShaderHashKey].name;
}

ShaderData* ShaderLoader::getShader(size_t ShaderHashKey) {
    return &myShaderSources[ShaderHashKey];
}

std::vector<std::pair<std::string,size_t>> ShaderLoader::getShaderList() {
    std::vector<std::pair<std::string,size_t>> toRet;
    for( const auto &[ShaderKey,ShaderData] : myShaderSources ) {
        toRet.emplace_back( ShaderData.name, ShaderKey );
    }
    return toRet;
}