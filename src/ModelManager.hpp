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

class ModelManager {
public:
	ModelManager(FilepathManager* inFilepathManager);
	void loadAndBindModel(const std::string& filename, tinygltf::Model& model, std::pair<GLuint, std::map<int, GLuint>>* inVaosAndEbos);
	void doLoadBindHashModel(const std::string& filename);
	bool loadModel(const std::string& filename, tinygltf::Model& model);

    void drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node);
	void drawModelNodesByHash(size_t inHashKey, int inSceneKey, const tinygltf::Scene& inScene);
	void drawModel(GLuint shaderProgram, float rotation);
	void drawModel(GLuint shaderProgram, tinygltf::Model inModel, glm::mat4 transform);
	void drawModelFromHash(GLuint shaderProgram, size_t inHashKey);
	void drawModelFromRenderObject(GLuint shaderProgram, RenderObject* inRenderObject);

	void loadModelButton();
	void drawModelLoaded(GLuint shaderProgram);


	void RenderModelInspectorWindow(tinygltf::Model* ModelData);

    // bind models
	void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node);
    std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model& model);

	std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos, vaoAndEbosB, vaoAndEbosLoaded;

	std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>> myVaosAndEbos;
	std::unordered_map<size_t,tinygltf::Model> myModels;
	size_t myHash;

	tinygltf::Model myModel, myModelB, myModelLoaded;
private:
	bool isModelLoaded;
	FilepathManager* myFilepathManager;
};
