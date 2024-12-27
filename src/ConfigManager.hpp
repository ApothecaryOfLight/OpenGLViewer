#pragma once

#include "FilepathManager.hpp"

#include <vector>
#include <string>

class ConfigManager{
public:
    ConfigManager(FilepathManager* inFilepathManager);
    std::vector<std::string> myModels, myShaders, myScenes;
    std::string myDefaultScene, myDefaultShader;
    FilepathManager* myFilepathManager;
private:
};
