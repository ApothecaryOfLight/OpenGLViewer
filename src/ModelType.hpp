#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tinygltf/tiny_gltf.h"

#include <vector>

struct NodeMeshData {
    GLuint vao;                       // The VAO for this node
    std::map<int, GLuint> vbos;       // VBOs for this node
};

struct ModelData {
    std::map<int, NodeMeshData> nodeMeshMap;
    tinygltf::Model gltfModel;
    //Animations* myAnimations;
};
