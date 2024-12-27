
#include "ModelLoader.hpp"
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

#include <iostream>
#include <string>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

ModelLoader::ModelLoader(FilepathManager* inFilepathManager) {
    myFilepathManager = inFilepathManager;
    myModelBind = new ModelBind();
}

void ModelLoader::loadAndBindModel(const std::string& inFilename, tinygltf::Model& inModel, std::pair<GLuint, std::map<int, GLuint>>* inVaosAndEbos) {
    std::string myModelsFilePath( myFilepathManager->myModelDir + inFilename );
    std::cout << "Loading: " << myModelsFilePath << std::endl;
    if (!loadModel(myModelsFilePath.c_str(), inModel)) {
        std::cerr << "Failed to load glTF model " << inFilename << std::endl;
    }
    std::pair<GLuint, std::map<int, GLuint>> tempVaosAndEbos = myModelBind->bindModel(inModel);
    *inVaosAndEbos = tempVaosAndEbos;
}

void ModelLoader::doLoadBindHashModel(const std::string& inFilename, std::unordered_map<size_t,tinygltf::Model>& myModels, std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>>& myVaosAndEbos) {
    tinygltf::Model myTempModel;
    std::string myModelFilePath( myFilepathManager->myModelDir + inFilename );
    std::cout << "MyModelFilePath " << myModelFilePath << std::endl;
    if (!loadModel(myModelFilePath.c_str(), myTempModel)) {
        std::cerr << "Failed to load glTF model" << std::endl;
    } else {
        std::pair<GLuint, std::map<int, GLuint>> myLocalVaoAndEbos = myModelBind->bindModel(myTempModel);
        size_t myLocalHash = std::hash<std::string>{}(std::string(inFilename));
        std::cout << "Adding model " << myLocalHash << std::endl;
        myModels[myLocalHash] = std::move(myTempModel);
        myVaosAndEbos[myLocalHash] = std::move(myLocalVaoAndEbos);
    } 
}

// Function to load a glTF model
bool ModelLoader::loadModel(const std::string& filename, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "WARNING: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "ERROR: " << err << std::endl;
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }
    return ret;
}

void ModelLoader::loadOGLModel(const std::string& path, std::vector<ModelData>& myOGLModels) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadASCIIFromFile(&model, &err, &warn, path)) {
        std::cerr << "Failed to load GLTF model: " << err << std::endl;
        return;
    }

    ModelData modelData;
    modelData.gltfModel = model;

    // Bind nodes and meshes
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        myModelBind->bindOGLModelNodes(modelData.nodeMeshMap, model, model.nodes[nodeIndex]);
    }

    // Store the model data
    myOGLModels.push_back(std::move(modelData));
}