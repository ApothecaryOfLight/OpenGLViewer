#include "SceneManager.hpp"
#include "SceneLoader.hpp"

#include "FilepathManager.hpp"

#include <iostream>

SceneManager::SceneManager(FilepathManager* inFilepathManager) {
    myFilepathManager = inFilepathManager;
    std::string mySceneDir = myFilepathManager->mySceneDir;

    mySceneLoader = new SceneLoader();

    size_t scene_hash_key = std::hash<std::string>{}(std::string("Default Scene"));
    std::cout << "Indexing scene " << scene_hash_key << std::endl;
    myScenes[scene_hash_key] = mySceneLoader->doLoadScene(myFilepathManager->mySceneDir + "default.scenexml");
}

SceneManager::~SceneManager() {
    delete mySceneLoader;
}

Scene* SceneManager::getScene(size_t SceneHashKey) {
    return myScenes[SceneHashKey];
}