#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include "InputManager.hpp"

#include "OpenGLManager.hpp"

InputManager::InputManager(OpenGLManager* inGLManager, std::vector<Shape*> *inshapes) {
    myGLManager = inGLManager;
    for (int i = 0; i < 1024; i++) {
        keys[i] = false;
    }
    shapes = inshapes;
    mouse_navigation = false;
    isTKeyPressed = false;
}
void InputManager::handleKeys(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        keys[event.key.keysym.scancode] = true;
    }
    else if (event.type == SDL_KEYUP) {
        keys[event.key.keysym.scancode] = false;
    }
    else if (event.type == SDL_MOUSEMOTION) {
        mouse_callback(event);
    }
}

void InputManager::updateCamera() {
    if (keys[SDL_SCANCODE_W])
        myGLManager->cameraPos += myGLManager->cameraSpeed * myGLManager->cameraFront;
    if (keys[SDL_SCANCODE_S])
        myGLManager->cameraPos -= myGLManager->cameraSpeed * myGLManager->cameraFront;
    if (keys[SDL_SCANCODE_A])
        myGLManager->cameraPos -= glm::normalize(glm::cross(myGLManager->cameraFront, myGLManager->cameraUp)) * myGLManager->cameraSpeed;
    if (keys[SDL_SCANCODE_D])
        myGLManager->cameraPos += glm::normalize(glm::cross(myGLManager->cameraFront, myGLManager->cameraUp)) * myGLManager->cameraSpeed;
    if (keys[SDL_SCANCODE_T]) {
        SDL_bool isMouseFree = SDL_GetRelativeMouseMode();
        if (isTKeyPressed == false) {
            isTKeyPressed = true;
            if (isMouseFree == SDL_TRUE) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                mouse_navigation = false;
            }
            else {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                mouse_navigation = true;
            }
        }
    }
    else if (!keys[SDL_SCANCODE_T]) {
        isTKeyPressed = false;
    }
}

bool InputManager::processInput() {
    if (keys[SDL_SCANCODE_ESCAPE]) {
        return false;
    }
    return true;
}

void InputManager::mouse_callback(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION && mouse_navigation) {
        float xoffset = event.motion.xrel;
        float yoffset = -event.motion.yrel; // Reversed since y-coordinates go from bottom to top

        xoffset *= myGLManager->sensitivity;
        yoffset *= myGLManager->sensitivity;

        myGLManager->yaw += xoffset;
        myGLManager->pitch += yoffset;

        // Constrain the pitch
        if (myGLManager->pitch > 89.0f)
            myGLManager->pitch = 89.0f;
        if (myGLManager->pitch < -89.0f)
            myGLManager->pitch = -89.0f;

        // Update the camera front vector
        glm::vec3 front;
        front.x = cos(glm::radians(myGLManager->yaw)) * cos(glm::radians(myGLManager->pitch));
        front.y = sin(glm::radians(myGLManager->pitch));
        front.z = sin(glm::radians(myGLManager->yaw)) * cos(glm::radians(myGLManager->pitch));
        myGLManager->cameraFront = glm::normalize(front);
    }
}

glm::vec3 InputManager::generateRay() {
    return glm::normalize(myGLManager->cameraFront); // The direction of the ray is the camera's front vector
}

bool InputManager::intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t) {
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

void InputManager::highlightIntersectedObject() {
    glm::vec3 rayOrigin = myGLManager->cameraPos;
    glm::vec3 rayDirection = generateRay();

    float closestT = std::numeric_limits<float>::max();
    Shape* closestShape = nullptr;

    for (Shape* shape : *shapes) {
        glm::vec3 aabbMin, aabbMax;

        if (Square* mySquare = dynamic_cast<Square*>(shape)) {
            aabbMin = mySquare->myPos - glm::vec3(0.5f);
            aabbMax = mySquare->myPos + glm::vec3(0.5f);
        }
        else if (Plane* plane = dynamic_cast<Plane*>(shape)) {
            aabbMin = plane->myPos - glm::vec3(50.0f, 0.0f, 50.0f);
            aabbMax = plane->myPos + glm::vec3(50.0f, 0.1f, 50.0f);
        }

        float t;
        if (intersectRayAABB(rayOrigin, rayDirection, aabbMin, aabbMax, t) && t < closestT) {
            closestT = t;
            closestShape = shape;
        }
        else {
            shape->isHighlighted = false;
        }
    }

    if (closestShape) {
        closestShape->isHighlighted = true;
    }
}
