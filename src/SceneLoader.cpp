
#include "SceneLoader.hpp"

#include <tinyxml2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional> // For std::hash

// Assuming RenderObject and Scene classes are already defined
RenderObject::RenderObject() : ModelHashKey(0), myTransformation(1.0f) {}

RenderObject::RenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, size_t inModelHashKey) 
    : ModelHashKey(inModelHashKey) {
    myTransformation = glm::translate(glm::mat4(1.0f), inTranslation);
    myTransformation = glm::rotate(myTransformation, glm::radians(inRotationIntensity), inRotation);
    myTransformation = glm::scale(myTransformation, inScale);
}

Scene::Scene() {

}

void Scene::addRenderObject(glm::vec3 inTranslation, float inRotationIntensity, glm::vec3 inRotation, glm::vec3 inScale, size_t inModelHashKey) {
    myRenderObjects.emplace_back(inTranslation, inRotationIntensity, inRotation, inScale, inModelHashKey);
}

// SceneLoader implementation
SceneLoader::SceneLoader() {

}

Scene* SceneLoader::doLoadScene(std::string inFileLoc) {
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
        
        if (!modelName || !positionText || !rotationText || !scaleText) {
            std::cerr << "Incomplete renderObject definition.\n";
            continue;
        }

        // Hash the model name for ModelHashKey
        size_t modelHashKey = std::hash<std::string>{}(modelName);

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

        // Add RenderObject to the Scene
        sceneToRet->addRenderObject(position, rotationIntensity, rotationAxis, scale, modelHashKey);
    }
    return sceneToRet;
}