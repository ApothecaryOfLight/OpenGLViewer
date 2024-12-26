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

#include "FilepathManager.hpp"

class OpenGLManager {
public:
    OpenGLManager(FilepathManager* inFilepathManager);
    GLuint myShaderProgram, myShaderB;
    GLuint VBO, VAO, shaderProgram;
    GLuint cubeVAO, cubeVBO;

    void InitializeOpenGL();
    void setCamera();

    void ClearScreen();
    void UpdateViewAndProjectionMatrices();
    void RenderB(float angle);

    void setupLighting();

    void renderShaderMenu();

    glm::mat4 view, proj;

    glm::vec3 cameraPos, cameraFront, cameraUp;
    float yaw, pitch;
    float lastX, lastY; // Assume initial mouse position at the center
    bool firstMouse;
    float cameraSpeed;
    float sensitivity; // Mouse sensitivity

    bool isCelShading;
private:
    ShapesManager* myShapesManager;
    ShaderManager* myShaderManager;
    int selectedShaderIndex; // Index of the selected shader in the dropdown
    FilepathManager* myFilepathManager;
};