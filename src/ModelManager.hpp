#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tinygltf/tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "FilepathManager.hpp"
#include "SceneLoader.hpp"
#include "ConfigManager.hpp"

#include "ModelType.hpp"
#include "ModelLoader.hpp"
#include "ModelDraw.hpp"

/*
RenderObjects have a size_t ModelHashKey that will point here
That will be a key to get the 'Model', a data structure that will contain Mesh nodes.
Each mesh node will have a vao, vbo, and any relevant material (and possibly texture? uv mapping?) data.
*/

class ModelManager {
public:
	ModelManager(ConfigManager* inConfigManager);



	void loadModelButton();
	void RenderModelInspectorWindow(tinygltf::Model* ModelData);

	std::vector<ModelData> myOGLModels;
	std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos, vaoAndEbosB, vaoAndEbosLoaded;
	std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>> myVaosAndEbos;
	std::unordered_map<size_t,tinygltf::Model> myModels;
	std::unordered_map<size_t,ModelData> myModelDatas;
	size_t myHash;
	tinygltf::Model myModel, myModelB, myModelLoaded;
	
	ModelDraw* myModelDrawer;
private:
	bool isModelLoaded;
	ConfigManager* myConfigManager;
	FilepathManager* myFilepathManager;
	ModelLoader* myModelLoader;
};
