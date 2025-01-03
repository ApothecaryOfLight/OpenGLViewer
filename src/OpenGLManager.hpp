#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shapes.hpp"
#include "ShaderManager.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ConfigManager.hpp"
#include "FilepathManager.hpp"

#include "SceneManager.hpp"

#include "ModelManager.hpp"

class OpenGLManager {
public:
    OpenGLManager(ConfigManager* inConfigManager);
    GLuint myShaderProgram, myShaderB;
    GLuint VBO, VAO;
    //GLuint shaderProgram;
    GLuint cubeVAO, cubeVBO;

    void InitializeOpenGL();
    void setCamera();

    void ClearScreen();
    void UpdateViewAndProjectionMatrices();
    void RenderB(float angle);

    void setupLighting();

    void renderShaderMenu();

    void doDrawScene(size_t inSceneHashKey);

    void doPrototypeDrawCall(float inAngle);
    void loadModelButton();

    void highlightIntersectedRenderObjectBoundingBox(glm::vec3 rayOrigin, glm::vec3 rayDirection);

    glm::mat4 view, proj;

    glm::vec3 cameraPos, cameraFront, cameraUp;
    float yaw, pitch;
    float lastX, lastY; // Assume initial mouse position at the center
    bool firstMouse;
    float cameraSpeed;
    float sensitivity; // Mouse sensitivity

    bool isCelShading;

    GLuint shaderProgram();
private:
    ConfigManager* myConfigManager;
    ShapesManager* myShapesManager;
    ShaderManager* myShaderManager;
    SceneManager* mySceneManager;
    int selectedShaderIndex; // Index of the selected shader in the dropdown
    FilepathManager* myFilepathManager;
    ModelManager* myModelManager;
};