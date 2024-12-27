#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

class RenderObject {
public:
    RenderObject();
    RenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, glm::vec3 inColor, size_t inShaderHashKey, size_t inModelHashKey);
    size_t ModelHashKey, ShaderHashKey;
    glm::mat4 myTransformation;
    glm::vec3 myColor;
    bool isHighlighted;
};

class Scene {
public:
    Scene();
    void addRenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, glm::vec3 inColor, size_t inShaderHashKey, size_t inModelHashKey);
    std::vector<RenderObject> myRenderObjects;
	std::unordered_map<size_t,std::vector<size_t>> myShaderRenderObjectVectors;
};

class SceneLoader {
public:
    SceneLoader();
    Scene* doLoadScene(std::string inFileLoc);
};