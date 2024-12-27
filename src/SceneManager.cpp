#include "SceneManager.hpp"
#include "SceneLoader.hpp"

#include "ConfigManager.hpp"
#include "FilepathManager.hpp"

#include <iostream>

SceneManager::SceneManager(ConfigManager* inConfigManager) {
    myConfigManager = inConfigManager;
    myFilepathManager = myConfigManager->myFilepathManager;
    std::string mySceneDir = myFilepathManager->mySceneDir;

    mySceneLoader = new SceneLoader();

    for( auto& mySceneFileLocation : myConfigManager->myScenes ) {
        size_t scene_hash_key = std::hash<std::string>{}(std::string("default.scenexml"));
        std::cout << "Indexing scene " << scene_hash_key << std::endl;
        myScenes[scene_hash_key] = mySceneLoader->doLoadScene(myFilepathManager->mySceneDir + "default.scenexml");
    }
}

SceneManager::~SceneManager() {
    delete mySceneLoader;
}

Scene* SceneManager::getScene(size_t SceneHashKey) {
    return myScenes[SceneHashKey];
}