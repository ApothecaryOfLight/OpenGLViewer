
#include "OpenGLManager.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shapes.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

#include "FilepathManager.hpp"
#include "SceneManager.hpp"
#include "SceneLoader.hpp"

OpenGLManager::OpenGLManager(ConfigManager* inConfigManager) {
    selectedShaderIndex = 0;
    myConfigManager = inConfigManager;
    myFilepathManager = myConfigManager->myFilepathManager;
    InitializeOpenGL();
}

void OpenGLManager::InitializeOpenGL() {
    std::cout << "Initializing OpenGL!" << std::endl;
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    mySceneManager = new SceneManager(myConfigManager);
    myShaderManager = new ShaderManager(myConfigManager);
    myShapesManager = new ShapesManager;
    myModelManager = new ModelManager(myConfigManager);

    isCelShading = false;
    //myShaderManager->loadShadersLight();
    //myShaderManager->loadShadersOutline();
    //shaderProgram = myShaderManager->loadShader();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    setCamera();
    std::cout << "OpenGL initialized!" << std::endl;
}

void OpenGLManager::setCamera() {
    cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraSpeed = 0.05f; // Adjust as needed

    //cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
    //cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    yaw = -90.0f; // Initially looking along the negative Z-axis
    pitch = 0.0f;
    lastX = 400;
    lastY = 300; // Assume initial mouse position at the center
    firstMouse = true;
    sensitivity = 0.1f; // Mouse sensitivity
}

/*
TODO: Connect this to a menu with sliders as well, live sense of what all these things are.
Also the place for a switchable lighting system maybe?
*/
void OpenGLManager::setupLighting() {
    // Light properties
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);

    GLint lightPosLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "light.position");
    GLint lightAmbientLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "light.ambient");
    GLint lightDiffuseLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "light.diffuse");
    GLint lightSpecularLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "light.specular");

    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
    glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
    glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));

    // Material properties
    glm::vec3 materialAmbient(0.0f, 1.0f, 0.0f);
    glm::vec3 materialDiffuse(0.0f, 1.0f, 0.0f);
    glm::vec3 materialSpecular(0.0f, 1.0f, 0.0f);
    float materialShininess = 64.0f;

    GLint materialAmbientLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "material.ambient");
    GLint materialDiffuseLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "material.diffuse");
    GLint materialSpecularLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "material.specular");
    GLint materialShininessLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "material.shininess");

    glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));
    glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));
    glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
    glUniform1f(materialShininessLoc, materialShininess);
}



void OpenGLManager::ClearScreen() {
    // Set the background color to blue
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Clear the screen to the background color (blue) and clear the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLManager::UpdateViewAndProjectionMatrices() {
    // Update the view matrix based on the camera position
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); //last param is clipping plane distance

    GLint viewLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    GLint viewPosLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));

    // Update view and projection matrices
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    if (isCelShading) {
        //BEGIN CELL SHADING BLOCK
        // Set light and view position
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 viewPos(0.0f, 0.0f, 3.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
        glUniform3fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "viewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "objectColor"), 1, glm::value_ptr(objectColor));
        // Update view and projection matrices
        glUniformMatrix4fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        //END CELL SHADING BLOCK
    }

}

void OpenGLManager::RenderB(float angle) {
    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Calculate transformation matrices
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);

    GLint modelLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(myShaderManager->myCurrentShaderProgramID, "proj");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // Draw the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Draw the cube
    model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void OpenGLManager::renderShaderMenu() {
    std::vector<std::pair<std::string,size_t>> myShaderList = myShaderManager->getShaderList();
    // Begin the menu window
    ImGui::Begin("Shader Menu");
    // Render the dropdown
    if (ImGui::BeginCombo("Select Shader", myShaderList[selectedShaderIndex].first.c_str())) {
        for (int i = 0; i < myShaderList.size(); ++i) {
            const bool isSelected = (selectedShaderIndex == i);
            if (ImGui::Selectable(myShaderList[i].first.c_str(), isSelected)) {
                selectedShaderIndex = i; // Update selected index
            }
            // Set the initial focus when opening the combo
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Render the Apply button
    if (ImGui::Button("Apply")) {
        myShaderManager->setShader(myShaderList[selectedShaderIndex].second);
        setupLighting();
        UpdateViewAndProjectionMatrices();
    }

    // End the menu window
    ImGui::End();
}

void OpenGLManager::doDrawScene(size_t inSceneHashKey) {
    Scene* myScene = mySceneManager->getScene(inSceneHashKey);
    /*for( auto& myRenderObject : myScene->myRenderObjects ) {
        myModelManager->drawModelFromRenderObject(myShaderManager->myCurrentShaderProgramID, &myRenderObject);
    }*/

    for( auto& [ShaderHashKey,VectorOfRenderObjects] : myScene->myShaderRenderObjectVectors ) {
        //std::cout << "Drawing objects with shader " << ShaderHashKey << std::endl;
        myShaderManager->setShader(ShaderHashKey);
        setupLighting();
        UpdateViewAndProjectionMatrices();
        for( auto& myRenderObjectIndex : VectorOfRenderObjects ) {
            RenderObject* myRenderObject = &myScene->myRenderObjects[myRenderObjectIndex];
            //std::cout << "Drawing object with model hash of " << myRenderObject->ModelHashKey << std::endl;
            myModelManager->myModelDrawer->drawModelFromRenderObject(myShaderManager->getShader(ShaderHashKey), myRenderObject);
        }
    }
    myModelManager->myModelDrawer->drawAllOGLModels();
}

void OpenGLManager::doPrototypeDrawCall(float inAngle) {
    size_t hash_key_evergreen_tree = std::hash<std::string>{}("evergreen_tree.gltf");
    myModelManager->myModelDrawer->drawModel(myShaderManager->myCurrentShaderProgramID, inAngle*10);
    myModelManager->myModelDrawer->drawModelLoaded(myShaderManager->myCurrentShaderProgramID);
    myModelManager->myModelDrawer->drawModelFromHash(myShaderManager->myCurrentShaderProgramID, hash_key_evergreen_tree);
}

void OpenGLManager::loadModelButton() {
    myModelManager->loadModelButton();
}

GLuint OpenGLManager::shaderProgram() {
    return myShaderManager->myCurrentShaderProgramID;
}

bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t) {
    float t1 = (aabbMin.x - rayOrigin.x) / rayDirection.x;
    float t2 = (aabbMax.x - rayOrigin.x) / rayDirection.x;
    float t3 = (aabbMin.y - rayOrigin.y) / rayDirection.y;
    float t4 = (aabbMax.y - rayOrigin.y) / rayDirection.y;
    float t5 = (aabbMin.z - rayOrigin.z) / rayDirection.z;
    float t6 = (aabbMax.z - rayOrigin.z) / rayDirection.z;

    float tmin = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
    float tmax = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });

    if (tmax < 0 || tmin > tmax) {
        return false;
    }

    t = tmin;
    return true;
}

void OpenGLManager::highlightIntersectedRenderObjectBoundingBox(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    size_t scene_hash_key = std::hash<std::string>{}(std::string("default.scenexml"));
    Scene* myScene = mySceneManager->getScene(scene_hash_key);
    float closestT = std::numeric_limits<float>::max();
    RenderObject* closestRenderObject = nullptr;

    for (RenderObject& renderObject : myScene->myRenderObjects) {
        renderObject.isHighlighted = false;
        const tinygltf::Model& model = myModelManager->myModels[renderObject.ModelHashKey];

        // Calculate the AABB of the model (assume it's precomputed or extract vertices to compute it)
        glm::vec3 aabbMin(FLT_MAX), aabbMax(-FLT_MAX);
        for (const auto& mesh : model.meshes) {
            for (const auto& primitive : mesh.primitives) {
                // Access vertex data
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i) {
                    glm::vec3 vertex(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
                    aabbMin = glm::min(aabbMin, vertex);
                    aabbMax = glm::max(aabbMax, vertex);
                }
            }
        }

        // Transform the AABB to world space using the RenderObject's transformation matrix
        glm::vec3 worldAABBMin = glm::vec3(renderObject.myTransformation * glm::vec4(aabbMin, 1.0f));
        glm::vec3 worldAABBMax = glm::vec3(renderObject.myTransformation * glm::vec4(aabbMax, 1.0f));

        // Check for intersection with the transformed AABB
        float t;
        if (intersectRayAABB(rayOrigin, rayDirection, worldAABBMin, worldAABBMax, t) && t < closestT) {
            closestT = t;
            closestRenderObject = &renderObject;
        }
    }

    if( closestRenderObject != nullptr ) {
        closestRenderObject->isHighlighted = true;
    }
}