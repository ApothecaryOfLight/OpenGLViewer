#pragma once

#include <unordered_map>
#include "SceneLoader.hpp"

#include "ConfigManager.hpp"
#include "FilepathManager.hpp"

class SceneManager {
public:
	SceneManager(ConfigManager* inConfigManager);
	~SceneManager();
	Scene* getScene(size_t SceneHashKey);
private:
	SceneLoader* mySceneLoader;
	std::unordered_map<size_t,Scene*> myScenes;
	ConfigManager* myConfigManager;
	FilepathManager* myFilepathManager;
};