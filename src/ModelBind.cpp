#include "ModelBind.hpp"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tinygltf/tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "FilepathManager.hpp"
#include "SceneLoader.hpp"
#include "ConfigManager.hpp"

#include "ModelType.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

ModelBind::ModelBind() {
    
}

// bind models
void ModelBind::bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) {
            continue;
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (attrib.first.compare("TANGENT") == 0) vaa = 3;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }

        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1) {

                GLuint texid;
                glGenTextures(1, &texid);

                tinygltf::Image& image = model.images[tex.source];

                glBindTexture(GL_TEXTURE_2D, texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1) {
                    format = GL_RED;
                }
                else if (image.component == 2) {
                    format = GL_RG;
                }
                else if (image.component == 3) {
                    format = GL_RGB;
                }
                else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                }
                else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                }
                else {
                    // ???
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                    format, type, &image.image.at(0));
            }
        }
    }
}

void ModelBind::bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node) {
    std::cout << "Processing node with index: " << &node - &model.nodes[0] << std::endl;

    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "Binding mesh with index: " << node.mesh << std::endl;
        bindMesh(vbos, model, model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((node.children[i] >= 0) && (node.children[i] < static_cast<int>(model.nodes.size())));
        std::cout << "Processing child node with index: " << childIndex << std::endl;
        bindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<GLuint, std::map<int, GLuint>> ModelBind::bindModel(tinygltf::Model& model) {
    std::map<int, GLuint> vbos;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    std::cout << "Default scene contains " << scene.nodes.size() << " root nodes." << std::endl;
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;
        bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
    // cleanup vbos but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }

    return { vao, vbos };
}


ModelData ModelBind::bindOGLModel(tinygltf::Model& model) {
    ModelData myModelData;
    myModelData.gltfModel = model;

    std::cout << "Default scene contains " << model.scenes[model.defaultScene].nodes.size() << " root nodes." << std::endl;

    // Bind all nodes in the default scene
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;
        bindOGLModelNodes(myModelData.nodeMeshMap, model, model.nodes[nodeIndex]);
    }

    // Cleanup VBOs but do not delete index buffers yet
    for (auto it = myModelData.nodeMeshMap.begin(); it != myModelData.nodeMeshMap.end(); ++it) {
        for (auto vboIt = it->second.vbos.begin(); vboIt != it->second.vbos.end();) {
            tinygltf::BufferView bufferView = model.bufferViews[vboIt->first];
            if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
                glDeleteBuffers(1, &vboIt->second);
                vboIt = it->second.vbos.erase(vboIt);
            } else {
                ++vboIt;
            }
        }
    }

    return myModelData;
}

void ModelBind::bindOGLModelNodes(std::map<int, NodeMeshData>& nodeMeshMap, tinygltf::Model& model, tinygltf::Node& node) {
    NodeMeshData nodeMeshData;

    // Generate a VAO for this node
    glGenVertexArrays(1, &nodeMeshData.vao);
    glBindVertexArray(nodeMeshData.vao);

    std::cout << "Processing node with index: " << &node - &model.nodes[0] << std::endl;

    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "Binding mesh with index: " << node.mesh << std::endl;
        bindMesh(nodeMeshData.vbos, model, model.meshes[node.mesh]);
    }

    // Store the NodeMeshData in the map
    int nodeIndex = &node - &model.nodes[0];
    nodeMeshMap[nodeIndex] = nodeMeshData;

    // Traverse child nodes
    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((childIndex >= 0) && (childIndex < static_cast<int>(model.nodes.size())));
        std::cout << "Processing child node with index: " << childIndex << std::endl;
        bindOGLModelNodes(nodeMeshMap, model, model.nodes[childIndex]);
    }

    // Unbind the VAO to avoid accidental modification
    glBindVertexArray(0);
}
