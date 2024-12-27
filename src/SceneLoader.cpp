
#include "SceneLoader.hpp"

#include <tinyxml2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional> // For std::hash
#include <unordered_map>
#include <iomanip>

// Assuming RenderObject and Scene classes are already defined
RenderObject::RenderObject() : ModelHashKey(0), myTransformation(glm::mat4(1.0f)) {}

RenderObject::RenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, glm::vec3 inColor, size_t inShaderHashKey, size_t inModelHashKey) 
    : myTransformation(glm::mat4(1.0f)) {
    myTransformation = glm::translate(glm::mat4(1.0f), inTranslation);
    myTransformation = glm::rotate(myTransformation, glm::radians(inRotationIntensity), inRotation);
    myTransformation = glm::scale(myTransformation, inScale);
    myColor = inColor;
    ShaderHashKey = inShaderHashKey;
    ModelHashKey = inModelHashKey;
    isHighlighted = false;
}

Scene::Scene() {

}

void Scene::addRenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, glm::vec3 inColor, size_t inShaderHashKey, size_t inModelHashKey) {
    myRenderObjects.emplace_back(inTranslation, inRotationIntensity, inRotation, inScale, inColor, inShaderHashKey, inModelHashKey);
    myShaderRenderObjectVectors[inShaderHashKey].push_back( myRenderObjects.size()-1 );
}

// SceneLoader implementation
SceneLoader::SceneLoader() {

}

Scene* SceneLoader::doLoadScene(std::string inFileLoc) {
    std::cout << "Loading scene " << inFileLoc << std::endl;

    Scene* sceneToRet = new Scene();
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(inFileLoc.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load file: " << inFileLoc << "\n";
        return sceneToRet;
    }

    tinyxml2::XMLElement* sceneElement = doc.FirstChildElement("scene");
    if (!sceneElement) {
        std::cerr << "Scene element not found.\n";
        return sceneToRet;
    }

    tinyxml2::XMLElement* renderObjectsElement = sceneElement->FirstChildElement("renderObjects");
    if (!renderObjectsElement) {
        std::cerr << "RenderObjects element not found.\n";
        return sceneToRet;
    }

    for (tinyxml2::XMLElement* renderObjectElement = renderObjectsElement->FirstChildElement("renderObject");
         renderObjectElement != nullptr;
         renderObjectElement = renderObjectElement->NextSiblingElement("renderObject")) {

        const char* modelName = renderObjectElement->FirstChildElement("modelName")->GetText();
        const char* positionText = renderObjectElement->FirstChildElement("position")->GetText();
        const char* rotationText = renderObjectElement->FirstChildElement("rotation")->GetText();
        const char* scaleText = renderObjectElement->FirstChildElement("scale")->GetText();
        const char* colorText = renderObjectElement->FirstChildElement("color")->GetText();
        const char* shaderName = renderObjectElement->FirstChildElement("shader")->GetText();
        
        if (!modelName || !positionText || !rotationText || !scaleText) {
            std::cerr << "Incomplete renderObject definition.\n";
            continue;
        }

        // Hash the model name for ModelHashKey
        size_t modelHashKey = std::hash<std::string>{}(modelName);
        size_t shaderHashKey = std::hash<std::string>{}(shaderName);
        std::cout << "Giving render object " << modelName << " hash value of " << modelHashKey << " with shader hash key of " << shaderHashKey << std::endl;

        // Parse position
        glm::vec3 position;
        std::stringstream positionStream(positionText);
        positionStream >> position.x >> position.y >> position.z;

        // Parse rotation
        glm::vec3 rotationAxis;
        float rotationIntensity;
        std::stringstream rotationStream(rotationText);
        rotationStream >> rotationIntensity >> rotationAxis.x >> rotationAxis.y >> rotationAxis.z;

        // Parse scale
        glm::vec3 scale;
        std::stringstream scaleStream(scaleText);
        scaleStream >> scale.x >> scale.y >> scale.z;

        // Parse color
        glm::vec3 color;
        std::stringstream colorStream(colorText);
        colorStream >> color.x >> color.y >> color.z;
        std::cout << "Setting color to " << color.x << ", " << color.y << ", " << color.z << std::endl;

        // Add RenderObject to the Scene
        sceneToRet->addRenderObject(position, rotationIntensity, rotationAxis, scale, color, shaderHashKey, modelHashKey);
    }
    std::cout << "Loaded scene " << inFileLoc << std::endl;
    return sceneToRet;
}