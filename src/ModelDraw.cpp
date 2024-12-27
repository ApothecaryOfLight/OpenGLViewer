#include "ModelDraw.hpp"

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

// recursively draw node and children nodes of model
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

ModelDraw::ModelDraw(
    std::unordered_map<size_t,std::pair<GLuint, std::map<int, GLuint>>>& inVaosAndEbos,
    std::unordered_map<size_t,tinygltf::Model>& inModels,
    std::pair<GLuint, std::map<int, GLuint>>& invaoAndEbos,
    std::pair<GLuint, std::map<int, GLuint>>& invaoAndEbosLoaded,
    tinygltf::Model& inmyModel,
    tinygltf::Model& inmyModelLoaded,
    std::unordered_map<size_t,ModelData>& inmyModelDatas,
    bool& inisModelLoaded
) : myVaosAndEbos(inVaosAndEbos),
      myModels(inModels),
      vaoAndEbos(invaoAndEbos),
      vaoAndEbosLoaded(invaoAndEbosLoaded),
      myModel(inmyModel),
      myModelLoaded(inmyModelLoaded),
      myModelDatas(inmyModelDatas),
      isModelLoaded(inisModelLoaded)
{}

void ModelDraw::drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            vbos.at(indexAccessor.bufferView)
        );

        glDrawElements(
            primitive.mode,
            indexAccessor.count,
            indexAccessor.componentType,
            BUFFER_OFFSET(indexAccessor.byteOffset)
        );
    }
}

void ModelDraw::drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        drawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}

void ModelDraw::drawModelNodesByHash(size_t inHashKey, int inSceneKey, const tinygltf::Scene& inScene) {
    const auto& myLocalVaoAndEbos = myVaosAndEbos[inHashKey];
    tinygltf::Model& myLocalModel = myModels[inHashKey];

    // Get the current node
    const tinygltf::Node& node = myLocalModel.nodes[inScene.nodes[inSceneKey]];

    // Draw the mesh associated with the node
    if (node.mesh >= 0) {
        std::cout << "Drawing mesh with index: " << node.mesh << std::endl;
        drawMesh(myLocalVaoAndEbos.second, myLocalModel, myLocalModel.meshes[node.mesh]);
    }

    // Recursively draw child nodes
    for (size_t i = 0; i < node.children.size(); ++i) {
        int childIndex = node.children[i];
        std::cout << "Processing child node with index: " << childIndex << std::endl;
        drawModelNodesByHash(inHashKey, node.children[i], inScene);
    }
}


void ModelDraw::drawModel(GLuint shaderProgram, tinygltf::Model inModel, glm::mat4 transform) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(vaoAndEbos.first);
    const tinygltf::Scene& scene = inModel.scenes[inModel.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbos, inModel, inModel.nodes[scene.nodes[i]]);
    }
    glBindVertexArray(0);
}

void ModelDraw::drawModel(GLuint shaderProgram, float rotation) {
    glm::mat4 transformation = glm::mat4(1.0f); // Identity matrix
    transformation = glm::translate(transformation, glm::vec3(0.0f, 10.0f, -45.0f)); // Translation
    transformation = glm::rotate(transformation, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around Y-axis
    transformation = glm::scale(transformation, glm::vec3(5.0f, 5.0f, 5.0f)); // Scaling
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
    glBindVertexArray(vaoAndEbos.first);
    const tinygltf::Scene& scene = myModel.scenes[myModel.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbos, myModel, myModel.nodes[scene.nodes[i]]);
    }
    glBindVertexArray(0);
}



void ModelDraw::drawAllOGLModels() {
    for (size_t i = 0; i < myModelDatas.size(); ++i) {
        drawOGLModel(i);
    }
}

void ModelDraw::drawOGLModel(int modelIndex) {
    if (modelIndex < 0 || modelIndex >= static_cast<int>(myModelDatas.size())) {
        std::cerr << "Invalid model index: " << modelIndex << std::endl;
        return;
    }

    const ModelData& modelData = myModelDatas[modelIndex];
    const tinygltf::Scene& scene = modelData.gltfModel.scenes[modelData.gltfModel.defaultScene];

    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        drawOGLNode(nodeIndex, myModelDatas[modelIndex]);
    }
}

void ModelDraw::drawOGLNode(int nodeIndex, ModelData& inModelData) {
    const std::map<int, NodeMeshData>& nodeMeshMap = inModelData.nodeMeshMap;
    auto it = nodeMeshMap.find(nodeIndex);
    if (it != nodeMeshMap.end()) {
        const NodeMeshData& nodeMeshData = it->second;

        glBindVertexArray(nodeMeshData.vao);
        // Assuming drawMesh is a function that knows how to draw the mesh
        drawMesh(nodeMeshData.vbos, inModelData.gltfModel, inModelData.gltfModel.meshes[nodeIndex]);
        glBindVertexArray(0);
    }
}

void ModelDraw::drawModelLoaded(GLuint shaderProgram) {
    if (!isModelLoaded) {
        return;
    }
    glm::mat4 transformation = glm::mat4(1.0f); // Identity matrix
    transformation = glm::translate(transformation, glm::vec3(0.0f, 5.0f, -45.0f)); // Translation
    transformation = glm::rotate(transformation, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around Y-axis
    transformation = glm::scale(transformation, glm::vec3(5.0f, 5.0f, 5.0f)); // Scaling
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
    glBindVertexArray(vaoAndEbosLoaded.first);

    // Example uniform settings
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), objectColor.x, objectColor.y, objectColor.z);

    const tinygltf::Scene& scene = myModelLoaded.scenes[myModelLoaded.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbosLoaded, myModelLoaded, myModelLoaded.nodes[scene.nodes[i]]);
    }
    glBindVertexArray(0);
}

void ModelDraw::drawModelFromHash(GLuint shaderProgram, size_t inHash) {
    // Check if the model and VAO/EBO data exist in the maps
    auto modelIt = myModels.find(inHash);
    auto vaoIt = myVaosAndEbos.find(inHash);
    if (modelIt != myModels.end() && vaoIt != myVaosAndEbos.end()) {
        tinygltf::Model& myLocalModel = modelIt->second;
        const auto& myLocalVaoAndEbos = vaoIt->second;

        // Create the transformation matrix
        glm::mat4 transformation = glm::mat4(1.0f); // Identity matrix
        transformation = glm::translate(transformation, glm::vec3(5.0f, 0.0f, -15.0f)); // Translation
        transformation = glm::rotate(transformation, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation
        transformation = glm::scale(transformation, glm::vec3(1.0f, 1.0f, 1.0f)); // Scaling

        // Pass the transformation matrix to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
        
        // Example uniform settings
        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 objectColor = glm::vec3(0.0f, 5.0f, 5.0f);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), objectColor.x, objectColor.y, objectColor.z);

        // Bind the VAO
        glBindVertexArray(myLocalVaoAndEbos.first);

        // Access the scene from the model
        const tinygltf::Scene& scene = myLocalModel.scenes[myLocalModel.defaultScene];
        for (size_t i = 0; i < scene.nodes.size(); ++i) {
            drawModelNodesByHash(inHash, i, scene);
        }

        // Unbind the VAO
        glBindVertexArray(0);
    } else {
        std::cerr << "Error: Model or VAO/EBO data not found for hash: " << inHash << std::endl;
    }
}

void ModelDraw::drawModelFromRenderObject(GLuint shaderProgram, RenderObject* inRenderObject) {
    // Check if the model and VAO/EBO data exist in the maps
    size_t myRenderObjectHash = inRenderObject->ModelHashKey;
    auto modelIt = myModels.find(myRenderObjectHash);
    auto vaoIt = myVaosAndEbos.find(myRenderObjectHash);
    if (modelIt != myModels.end() && vaoIt != myVaosAndEbos.end()) {
        tinygltf::Model& myLocalModel = modelIt->second;
        const auto& myLocalVaoAndEbos = vaoIt->second;

        // Pass the transformation matrix to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(inRenderObject->myTransformation));
        
        glm::vec3 lightPos = glm::vec3(1.2f, 5.0f, 2.0f);
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 objectColor = inRenderObject->myColor;
        if( inRenderObject->isHighlighted ) {
            objectColor = glm::vec3(0.7f, 0.7f, 0.0f);
        }

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), objectColor.x, objectColor.y, objectColor.z);


        glm::vec3 lightAmbient = lightColor * glm::vec3(0.8f);  // Dimmer ambient light
        glm::vec3 lightDiffuse = lightColor * glm::vec3(0.8f);  // Medium diffuse light
        glm::vec3 lightSpecular = lightColor * glm::vec3(0.8f); // Bright specular light

        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), lightAmbient.x, lightAmbient.y, lightAmbient.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), lightSpecular.x, lightSpecular.y, lightSpecular.z);

        glm::vec3 materialAmbient = glm::vec3(1.0f, 1.0f, 1.0f); // Fully lit object
        glm::vec3 materialDiffuse = glm::vec3(1.0f, 1.0f, 1.0f); // White base color
        glm::vec3 materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f); // Shiny highlight
        float materialShininess = 32.0f; // Sharp specular highlights

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), materialAmbient.x, materialAmbient.y, materialAmbient.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), materialDiffuse.x, materialDiffuse.y, materialDiffuse.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), materialSpecular.x, materialSpecular.y, materialSpecular.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), materialShininess);


        // Bind the VAO
        glBindVertexArray(myLocalVaoAndEbos.first);

        // Access the scene from the model
        /*const tinygltf::Scene& scene = myLocalModel.scenes[myLocalModel.defaultScene];
        for (size_t i = 0; i < scene.nodes.size(); ++i) {
            drawModelNodesByHash(myRenderObjectHash, i, scene);
        }*/
        const tinygltf::Scene& scene = myLocalModel.scenes[myLocalModel.defaultScene];
        for (size_t i = 0; i < scene.nodes.size(); ++i) {
            int nodeIndex = scene.nodes[i]; // Get the actual node index from the scene
            drawModelNodesByHash(myRenderObjectHash, nodeIndex, scene);
        }

        // Unbind the VAO
        glBindVertexArray(0);
    } else {
        std::cerr << "Error: Model or VAO/EBO data not found for hash: " << myRenderObjectHash << std::endl;
    }
}