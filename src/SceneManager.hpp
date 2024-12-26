#pragma once

#include <unordered_map>
#include "SceneLoader.hpp"

#include "FilepathManager.hpp"

class SceneManager {
public:
	SceneManager(FilepathManager* inFilepathManager);
	~SceneManager();
	Scene* getScene(size_t SceneHashKey);
private:
	SceneLoader* mySceneLoader;
	std::unordered_map<size_t,Scene*> myScenes;
	FilepathManager* myFilepathManager;
};