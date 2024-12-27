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

#include "ModelType.hpp"
#include "ModelBind.hpp"
#include "ModelLoader.hpp"
#include "ModelDraw.hpp"

// Load the glTF model
ModelManager::ModelManager(ConfigManager* inConfigManager) {
    isModelLoaded = false;
    myConfigManager = inConfigManager;
    myFilepathManager = myConfigManager->myFilepathManager;
    std::string myModelsFilePath( myFilepathManager->myModelDir );

    myModelLoader = new ModelLoader(myFilepathManager);
    myModelDrawer = new ModelDraw(
        myVaosAndEbos,
        myModels,
        vaoAndEbos,
        vaoAndEbosLoaded,
        myModel,
        myModelLoaded,
        myModelDatas,
        isModelLoaded
    );

    myModelLoader->loadAndBindModel("monkey_head.gltf",myModel, &vaoAndEbos);
    myModelLoader->loadAndBindModel("tile.gltf",myModelB, &vaoAndEbosB);
    myModelLoader->loadAndBindModel("evergreen_tree.gltf",myModelLoaded,&vaoAndEbosLoaded);
    myModelLoader->loadOGLModel("evergreen_tree.gltf", myOGLModels);

    for( auto& myModelFilepath : myConfigManager->myModels ) {
        myModelLoader->doLoadBindHashModel(myModelFilepath, myModels, myVaosAndEbos);
    }
}







//Menu stuff
void ModelManager::loadModelButton() {
    std::cout << "Loading monkey head..." << std::endl;
    if (!myModelLoader->loadModel("../../bin/data/monkey_head.gltf", myModelLoaded)) {
        std::cerr << "Failed to load glTF model" << std::endl;
    }
    vaoAndEbosLoaded = myModelLoader->myModelBind->bindModel(myModelLoaded);

    isModelLoaded = true;
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
