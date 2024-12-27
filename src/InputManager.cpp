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
#include "SceneLoader.hpp"

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

bool rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                           const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
    const float EPSILON = 1e-8f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDirection, edge2);
    float a = glm::dot(edge1, h);

    if (fabs(a) < EPSILON) {
        return false; // Ray is parallel to the triangle
    }

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDirection, q);
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    t = f * glm::dot(edge2, q);
    return t > EPSILON; // Only return true if t is positive
}


void InputManager::highlightIntersectedRenderObjectRaytrace(std::vector<RenderObject*>& renderObjects, std::unordered_map<size_t, tinygltf::Model>& modelMap) {
    glm::vec3 rayOrigin = myGLManager->cameraPos;
    glm::vec3 rayDirection = generateRay();

    float closestT = std::numeric_limits<float>::max();
    RenderObject* closestRenderObject = nullptr;

    for (RenderObject* renderObject : renderObjects) {
        // Retrieve the model associated with this RenderObject
        auto modelIter = modelMap.find(renderObject->ModelHashKey);
        if (modelIter == modelMap.end()) {
            continue; // Skip if the model isn't found
        }

        const tinygltf::Model& model = modelIter->second;

        // Iterate over the mesh data in the model
        for (const auto& mesh : model.meshes) {
            for (const auto& primitive : mesh.primitives) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                // Iterate over triangles in the mesh
                for (size_t i = 0; i < accessor.count; i += 3) {
                    glm::vec3 v0 = glm::vec3(renderObject->myTransformation * glm::vec4(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2], 1.0f));
                    glm::vec3 v1 = glm::vec3(renderObject->myTransformation * glm::vec4(positions[(i + 1) * 3 + 0], positions[(i + 1) * 3 + 1], positions[(i + 1) * 3 + 2], 1.0f));
                    glm::vec3 v2 = glm::vec3(renderObject->myTransformation * glm::vec4(positions[(i + 2) * 3 + 0], positions[(i + 2) * 3 + 1], positions[(i + 2) * 3 + 2], 1.0f));

                    float t;
                    if (rayIntersectsTriangle(rayOrigin, rayDirection, v0, v1, v2, t) && t < closestT) {
                        closestT = t;
                        closestRenderObject = renderObject;
                    }
                }
            }
        }
    }

    // Highlight the closest intersected RenderObject
    for (RenderObject* renderObject : renderObjects) {
        renderObject->myColor = (renderObject == closestRenderObject) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f); // Red for highlight, white otherwise
    }
}

void InputManager::highlightIntersectedRenderObjectBoundingBox() {
    glm::vec3 rayOrigin = myGLManager->cameraPos;
    glm::vec3 rayDirection = generateRay();
    myGLManager->highlightIntersectedRenderObjectBoundingBox(rayOrigin,rayDirection);
}
