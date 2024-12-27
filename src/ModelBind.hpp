
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

class ModelBind {
public:
    ModelBind();
    
    // bind models
	void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node);
    std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model& model);

	ModelData bindOGLModel(tinygltf::Model& model);
	void bindOGLModelNodes(std::map<int, NodeMeshData>& nodeMeshMap, tinygltf::Model& model, tinygltf::Node& node);
};