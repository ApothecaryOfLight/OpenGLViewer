#include "ModelManager.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "FilepathManager.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ConfigManager.hpp"

// Load the glTF model
ModelManager::ModelManager(ConfigManager* inConfigManager) {
    isModelLoaded = false;
    myConfigManager = inConfigManager;
    myFilepathManager = myConfigManager->myFilepathManager;
    std::string myModelsFilePath( myFilepathManager->myModelDir );

    loadAndBindModel("monkey_head.gltf",myModel, &vaoAndEbos);
    loadAndBindModel("tile.gltf",myModelB, &vaoAndEbosB);
    loadAndBindModel("evergreen_tree.gltf",myModelLoaded,&vaoAndEbosLoaded);

    for( auto& myModelFilepath : myConfigManager->myModels ) {
        doLoadBindHashModel(myModelFilepath);
    }
}

void ModelManager::loadAndBindModel(const std::string& inFilename, tinygltf::Model& inModel, std::pair<GLuint, std::map<int, GLuint>>* inVaosAndEbos) {
    std::string myModelsFilePath( myFilepathManager->myModelDir + inFilename );
    std::cout << "Loading: " << myModelsFilePath << std::endl;
    if (!loadModel(myModelsFilePath.c_str(), inModel)) {
        std::cerr << "Failed to load glTF model " << inFilename << std::endl;
    }
    std::pair<GLuint, std::map<int, GLuint>> tempVaosAndEbos = bindModel(inModel);
    *inVaosAndEbos = tempVaosAndEbos;
}

void ModelManager::doLoadBindHashModel(const std::string& inFilename) {
    tinygltf::Model myTempModel;
    std::string myModelFilePath( myFilepathManager->myModelDir + inFilename );
    std::cout << "MyModelFilePath " << myModelFilePath << std::endl;
    if (!loadModel(myModelFilePath.c_str(), myTempModel)) {
        std::cerr << "Failed to load glTF model" << std::endl;
    } else {
        std::pair<GLuint, std::map<int, GLuint>> myLocalVaoAndEbos = bindModel(myTempModel);
        size_t myLocalHash = std::hash<std::string>{}(std::string(inFilename));
        std::cout << "Adding model " << myLocalHash << std::endl;
        myModels[myLocalHash] = std::move(myTempModel);
        myVaosAndEbos[myLocalHash] = std::move(myLocalVaoAndEbos);
    } 
}

// Function to load a glTF model
bool ModelManager::loadModel(const std::string& filename, tinygltf::Model& model) {
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


// recursively draw node and children nodes of model
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
void ModelManager::drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
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

void ModelManager::drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        drawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}

void ModelManager::drawModelNodesByHash(size_t inHashKey, int inSceneKey, const tinygltf::Scene& inScene) {
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


void ModelManager::drawModel(GLuint shaderProgram, tinygltf::Model inModel, glm::mat4 transform) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(vaoAndEbos.first);
    const tinygltf::Scene& scene = inModel.scenes[inModel.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbos, inModel, inModel.nodes[scene.nodes[i]]);
    }
    glBindVertexArray(0);
}

void ModelManager::drawModel(GLuint shaderProgram, float rotation) {
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



// bind models
void ModelManager::bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) {
            continue;
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (attrib.first.compare("TANGENT") == 0) vaa = 3;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }

        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1) {

                GLuint texid;
                glGenTextures(1, &texid);

                tinygltf::Image& image = model.images[tex.source];

                glBindTexture(GL_TEXTURE_2D, texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1) {
                    format = GL_RED;
                }
                else if (image.component == 2) {
                    format = GL_RG;
                }
                else if (image.component == 3) {
                    format = GL_RGB;
                }
                else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                }
                else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                }
                else {
                    // ???
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                    format, type, &image.image.at(0));
            }
        }
    }
}

void ModelManager::bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node) {
    std::cout << "Processing node with index: " << &node - &model.nodes[0] << std::endl;

    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "Binding mesh with index: " << node.mesh << std::endl;
        bindMesh(vbos, model, model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((node.children[i] >= 0) && (node.children[i] < static_cast<int>(model.nodes.size())));
        std::cout << "Processing child node with index: " << childIndex << std::endl;
        bindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<GLuint, std::map<int, GLuint>> ModelManager::bindModel(tinygltf::Model& model) {
    std::map<int, GLuint> vbos;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    std::cout << "Default scene contains " << scene.nodes.size() << " root nodes." << std::endl;
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;
        bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
    // cleanup vbos but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }

    return { vao, vbos };
}

void ModelManager::loadModelButton() {
    std::cout << "Loading monkey head..." << std::endl;
    if (!loadModel("../../bin/data/monkey_head.gltf", myModelLoaded)) {
        std::cerr << "Failed to load glTF model" << std::endl;
    }
    vaoAndEbosLoaded = bindModel(myModelLoaded);

    isModelLoaded = true;
}

void ModelManager::drawModelLoaded(GLuint shaderProgram) {
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

void ModelManager::drawModelFromHash(GLuint shaderProgram, size_t inHash) {
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

void ModelManager::drawModelFromRenderObject(GLuint shaderProgram, RenderObject* inRenderObject) {
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

void ModelManager::RenderModelInspectorWindow(tinygltf::Model* ModelData) {
    /*static char searchBuffer[128] = ""; // For search functionality
    static tinygltf::Model* editableModel = ModelData; // Editable copy of the model

    ImGui::Begin("Model Inspector");

    // Search field
    ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));

    // Display model name
    if (ImGui::CollapsingHeader("General Info")) {
        ImGui::Text("Model Name: %s", ModelData->name.c_str());
        ImGui::Text("VAO: %u", ModelData->vao);
        ImGui::Text("EBO: %u", ModelData->ebo);
    }

    // Position
    if (ImGui::CollapsingHeader("Transformations")) {
        ImGui::InputFloat3("Position", editableModel.position);
        ImGui::InputFloat3("Scale", editableModel.scale);
        ImGui::InputFloat3("Rotation", editableModel.rotation);
    }

    // Apply Changes button
    if (ImGui::Button("Apply Changes")) {
        *model = editableModel; // Copy changes back to the original model
        // Add any logic to update OpenGL state or related structures here
    }

    ImGui::End();*/
}
