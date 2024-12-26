#include "FilepathManager.hpp"

#include <iostream>
#include <filesystem>

FilepathManager::FilepathManager() {
    try {
        // Retrieve the current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();
        
        // Output the current working directory
        myFilepath.assign(currentPath.string());
        mySceneDir.assign( myFilepath + "/data/scenes/");
        myModelDir.assign( myFilepath + "/data/models/");
        myShaderDir.assign( myFilepath + "/data/shaders/");

        std::cout << "Working directory: " << myFilepath << std::endl;
        std::cout << "Scenes Direcotry: " << mySceneDir << std::endl;
        std::cout << "Models Direcotry: " << myModelDir << std::endl;
        std::cout << "Shaders Direcotry: " << myShaderDir << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        // Handle any filesystem-related errors
        std::cerr << "Error when establishing working directory: " << e.what() << std::endl;
    }

    return;
}