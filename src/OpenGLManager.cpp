
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

OpenGLManager::OpenGLManager(FilepathManager* inFilepathManager) {
    selectedShaderIndex = 0;
    myFilepathManager = inFilepathManager;
}

void OpenGLManager::InitializeOpenGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    myShapesManager = new ShapesManager;
    myShaderManager = new ShaderManager(myFilepathManager);

    isCelShading = false;
    myShaderManager->loadShadersLight();
    //myShaderManager->loadShadersOutline();
    shaderProgram = myShaderManager->loadShader();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    setCamera();
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

    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "light.position");
    GLint lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
    GLint lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
    GLint lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");

    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
    glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
    glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));

    // Material properties
    glm::vec3 materialAmbient(0.0f, 1.0f, 0.0f);
    glm::vec3 materialDiffuse(0.0f, 1.0f, 0.0f);
    glm::vec3 materialSpecular(0.0f, 1.0f, 0.0f);
    float materialShininess = 64.0f;

    GLint materialAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    GLint materialDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    GLint materialSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    GLint materialShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

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

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
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
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(objectColor));
        // Update view and projection matrices
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
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

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "proj");

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
        shaderProgram = myShaderManager->getShader(myShaderList[selectedShaderIndex].second);
        setupLighting();
        UpdateViewAndProjectionMatrices();
    }

    // End the menu window
    ImGui::End();
}