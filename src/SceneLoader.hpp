#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RenderObject {
public:
    RenderObject();
    RenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, size_t inModelHashKey);
    size_t ModelHashKey;
    glm::mat4 myTransformation;
};

class Scene {
public:
    Scene();
    void addRenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, size_t inModelHashKey);
    std::vector<RenderObject> myRenderObjects;
};

class SceneLoader {
public:
    SceneLoader();
    Scene* doLoadScene(std::string inFileLoc);
};