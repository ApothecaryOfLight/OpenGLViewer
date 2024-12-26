#include "SceneManager.hpp"
#include "SceneLoader.hpp"

SceneManager::SceneManager() {
    mySceneLoader = new SceneLoader();

    size_t scene_hash_key = std::hash<std::string>{}("default");
    myScenes[scene_hash_key] = mySceneLoader->doLoadScene("../../bin/data/scenes/default.scenexml");
}

SceneManager::~SceneManager() {
    delete mySceneLoader;
}

Scene* SceneManager::getScene(size_t SceneHashKey) {
    return myScenes[SceneHashKey];
}