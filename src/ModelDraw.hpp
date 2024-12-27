
#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "SceneLoader.hpp"
#include "ModelType.hpp"

class ModelDraw {
public:
    ModelDraw(
        std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>>& inVaosAndEbos,
        std::unordered_map<size_t,tinygltf::Model>& inModels,
        std::pair<GLuint, std::map<int, GLuint>>& invaoAndEbos,
        std::pair<GLuint, std::map<int, GLuint>>& invaoAndEbosLoaded,
        tinygltf::Model& inmyModel,
        tinygltf::Model& inmyModelLoaded,
        std::unordered_map<size_t,ModelData>& inmyModelDatas,
        bool& inisModelLoaded
    );
    void drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node);
	void drawModelNodesByHash(size_t inHashKey, int inSceneKey, const tinygltf::Scene& inScene);
	void drawModel(GLuint shaderProgram, float rotation);
	void drawModel(GLuint shaderProgram, tinygltf::Model inModel, glm::mat4 transform);
	void drawModelFromHash(GLuint shaderProgram, size_t inHashKey);
	void drawModelFromRenderObject(GLuint shaderProgram, RenderObject* inRenderObject);
	void drawOGLModel(int modelIndex);
	void drawAllOGLModels();
	void drawOGLNode(int nodeIndex, ModelData& inModelData);
	void drawModelLoaded(GLuint shaderProgram);
private:
    std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>>& myVaosAndEbos;
    std::unordered_map<size_t,tinygltf::Model>& myModels;
    std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, &vaoAndEbosLoaded;
    tinygltf::Model& myModel, &myModelLoaded;
    bool &isModelLoaded;
	std::unordered_map<size_t,ModelData> &myModelDatas;
};