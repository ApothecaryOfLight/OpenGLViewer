#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "OpenGLManager.hpp"

class InputManager {
public:
    InputManager(OpenGLManager* inGLManager, std::vector<Shape*> *inshapes);
    bool keys[1024];

    void handleKeys(const SDL_Event& event);

    void updateCamera();

    bool processInput();

    void mouse_callback(const SDL_Event& event);

    void highlightIntersectedObject();
    bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t);
    glm::vec3 generateRay();
    OpenGLManager* myGLManager;
    std::vector<Shape*> *shapes;
private:
    bool mouse_navigation;
    bool isTKeyPressed;
};