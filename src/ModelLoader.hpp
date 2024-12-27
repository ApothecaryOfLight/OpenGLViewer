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
#include "ModelBind.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class ModelLoader{
public:
    ModelLoader(FilepathManager* inFilepathManager);
	void loadAndBindModel(const std::string& filename, tinygltf::Model& model, std::pair<GLuint, std::map<int, GLuint>>* inVaosAndEbos);
	void doLoadBindHashModel(const std::string& filename, std::unordered_map<size_t,tinygltf::Model>& myModels, std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>>& myVaosAndEbos);
	bool loadModel(const std::string& filename, tinygltf::Model& model);
	void loadOGLModel(const std::string& path, std::vector<ModelData>& myOGLModels);
	ModelBind* myModelBind;//Put back in private once the load model menu thingy is corrected.
private:
    FilepathManager* myFilepathManager;
};