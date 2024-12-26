#pragma once

#include <unordered_map>
#include "SceneLoader.hpp"

class SceneManager {
public:
	SceneManager();
	~SceneManager();
	Scene* getScene(size_t SceneHashKey);
private:
	SceneLoader* mySceneLoader;
	std::unordered_map<size_t,Scene*> myScenes;
};